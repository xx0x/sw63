#include "Communication.hpp"
#include "app/App.hpp"
#include "dev/System.hpp"
#include "tusb.h"
#include <cstring>
#include <string>

bool Communication::Init()
{
    // Initialize TinyUSB
    if (!tusb_init())
    {
        return false;
    }

    ResetReceiveBuffer();
    return true;
}

void Communication::Process()
{
    // Process TinyUSB tasks
    tud_task();

    // Check if USB is mounted and CDC is available
    if (!tud_cdc_connected())
    {
        return;
    }

    // Read available data
    while (tud_cdc_available())
    {
        uint8_t byte = tud_cdc_read_char();

        // Add byte to buffer
        if (rx_index_ < rx_buffer_.size())
        {
            rx_buffer_[rx_index_++] = byte;

            // Check if we have a complete message
            if (rx_index_ >= HEADER_SIZE)
            {
                uint8_t expected_length = rx_buffer_[1];
                if (rx_index_ >= HEADER_SIZE + static_cast<size_t>(expected_length))
                {
                    message_ready_ = true;
                }
            }
        }
        else
        {
            // Buffer overflow, reset
            ResetReceiveBuffer();
        }
    }

    // Process complete message if ready
    if (message_ready_)
    {
        Command command = static_cast<Command>(rx_buffer_[0]);
        uint8_t length = rx_buffer_[1];
        const uint8_t *data = &rx_buffer_[HEADER_SIZE];

        HandleMessage(command, data, length);
        ResetReceiveBuffer();
    }
}

bool Communication::IsConnected() const
{
    return tud_cdc_connected();
}

void Communication::HandleMessage(Command command, const uint8_t *data, uint8_t length)
{
    switch (command)
    {
    case Command::SET_TIME:
        HandleSetTime(data, length);
        break;
    case Command::GET_TIME:
        HandleGetTime();
        break;
    case Command::SET_CONFIG:
        HandleSetConfig(data, length);
        break;
    case Command::GET_CONFIG:
        HandleGetConfig();
        break;
    case Command::SET_CONFIG_OPTION:
        if (length == 2)
        {
            HandleSetConfigOption(data[0], data[1]);
        }
        else
        {
            SendResponse(command, Status::INVALID_LENGTH);
        }
        break;
    case Command::GET_CONFIG_OPTION:
        if (length == 1)
        {
            HandleGetConfigOption(data[0]);
        }
        else
        {
            SendResponse(command, Status::INVALID_LENGTH);
        }
        break;
    case Command::GET_BATTERY_LEVEL:
        HandleGetBatteryLevel();
        break;
    case Command::DISPLAY_TIME:
        HandleDisplayTime();
        break;
    case Command::DISPLAY_INTRO:
        HandleDisplayIntro();
        break;
    case Command::GET_CONFIG_OPTION_VALUES:
        if (length == 1)
        {
            HandleGetConfigOptionValues(data[0]);
        }
        else
        {
            SendResponse(command, Status::INVALID_LENGTH);
        }
        break;
    case Command::GET_VERSION:
        if (length == 0)
        {
            HandleGetVersion();
        }
        else
        {
            SendResponse(command, Status::INVALID_LENGTH);
        }
        break;
    default:
        SendResponse(command, Status::INVALID_COMMAND);
        break;
    }
}

void Communication::SendResponse(Command command, Status status, const uint8_t *data, uint8_t length)
{
    // Send response as [Command][Status][Length][Data...].
    // Write header and payload separately and handle partial writes from TinyUSB.
    uint8_t response_header[3] = {
        static_cast<uint8_t>(command),
        static_cast<uint8_t>(status),
        length,
    };

    size_t header_written = 0;
    while (header_written < sizeof(response_header))
    {
        header_written += tud_cdc_write(response_header + header_written,
                                        sizeof(response_header) - header_written);
        if (header_written < sizeof(response_header))
        {
            tud_cdc_write_flush();
            tud_task();
        }
    }

    if (data && length > 0)
    {
        size_t payload_written = 0;
        while (payload_written < length)
        {
            payload_written += tud_cdc_write(data + payload_written, length - payload_written);
            if (payload_written < length)
            {
                tud_cdc_write_flush();
                tud_task();
            }
        }
    }

    tud_cdc_write_flush();
}

void Communication::HandleSetTime(const uint8_t *data, uint8_t length)
{
    if (length != sizeof(DS3231::DateTime))
    {
        SendResponse(Command::SET_TIME, Status::INVALID_LENGTH);
        return;
    }

    // Parse the DateTime structure from the received data
    DS3231::DateTime datetime;
    std::memcpy(&datetime, data, sizeof(DS3231::DateTime));

    // Validate the datetime values
    if (datetime.hour > 23 || datetime.minute > 59 || datetime.second > 59 ||
        datetime.day < 1 || datetime.day > 31 ||
        datetime.month < 1 || datetime.month > 12 ||
        datetime.year < 2000 || datetime.year > 2099)
    {
        SendResponse(Command::SET_TIME, Status::INVALID_DATA);
        return;
    }

    // Set the time on the RTC
    bool success = App::rtc.SetDateTime(datetime);

    if (success)
    {
        SendResponse(Command::SET_TIME, Status::OK);
        // Force display the time
        App::ChangeLayer(Layer::Type::NORMAL);
    }
    else
    {
        SendResponse(Command::SET_TIME, Status::ERROR);
    }
}

void Communication::HandleGetTime()
{
    auto datetime_opt = App::rtc.GetDateTime();

    if (datetime_opt.has_value())
    {
        DS3231::DateTime datetime = datetime_opt.value();
        SendResponse(Command::GET_TIME, Status::OK,
                     reinterpret_cast<const uint8_t *>(&datetime),
                     sizeof(DS3231::DateTime));
    }
    else
    {
        SendResponse(Command::GET_TIME, Status::ERROR);
    }
}

void Communication::HandleSetConfig(const uint8_t *data, uint8_t length)
{
    if (length != sizeof(Settings::Config))
    {
        SendResponse(Command::SET_CONFIG, Status::INVALID_LENGTH);
        return;
    }

    // Parse the Config structure from the received data
    Settings::Config config;
    std::memcpy(&config, data, sizeof(Settings::Config));

    // Validate the config values
    // (Add validation as needed based on your Settings class constraints)

    // Set the configuration
    App::settings.SetConfig(config);

    SendResponse(Command::SET_CONFIG, Status::OK);
}

void Communication::HandleGetConfig()
{
    Settings::Config config = App::settings.GetConfig();
    SendResponse(Command::GET_CONFIG, Status::OK,
                 reinterpret_cast<const uint8_t *>(&config),
                 sizeof(Settings::Config));
}

void Communication::HandleSetConfigOption(uint8_t option, uint8_t value)
{
    Settings::Config config = App::settings.GetConfig();

    switch (option)
    {
    case 0:
        if (value >= Settings::GetSpeedCount())
        {
            SendResponse(Command::SET_CONFIG_OPTION, Status::INVALID_DATA);
            return;
        }
        config.speed = value;
        break;
    case 1:
        if (value >= static_cast<uint8_t>(Locale::Language::COUNT))
        {
            SendResponse(Command::SET_CONFIG_OPTION, Status::INVALID_DATA);
            return;
        }
        config.language = static_cast<Locale::Language>(value);
        break;
    case 2:
        if (value >= static_cast<uint8_t>(Display::NumStyle::COUNT))
        {
            SendResponse(Command::SET_CONFIG_OPTION, Status::INVALID_DATA);
            return;
        }
        config.num_style = static_cast<Display::NumStyle>(value);
        break;
    default:
        SendResponse(Command::SET_CONFIG_OPTION, Status::INVALID_DATA);
        return;
    }

    App::settings.SetConfig(config);
    SendResponse(Command::SET_CONFIG_OPTION, Status::OK);
}

void Communication::HandleGetConfigOption(uint8_t option)
{
    Settings::Config config = App::settings.GetConfig();
    uint8_t value = 0;

    switch (option)
    {
    case 0:
        value = config.speed;
        break;
    case 1:
        value = static_cast<uint8_t>(config.language);
        break;
    case 2:
        value = static_cast<uint8_t>(config.num_style);
        break;
    default:
        SendResponse(Command::GET_CONFIG_OPTION, Status::INVALID_DATA);
        return;
    }

    SendResponse(Command::GET_CONFIG_OPTION, Status::OK, &value, sizeof(value));
}

void Communication::HandleGetConfigOptionValues(uint8_t option)
{
    std::string options_text;

    switch (option)
    {
    case 0:
    {
        const size_t speed_count = Settings::GetSpeedCount();
        if (speed_count == 0)
        {
            options_text = "";
            break;
        }
        for (size_t i = 0; i < speed_count; ++i)
        {
            if (i > 0)
            {
                options_text += ';';
            }
            options_text += std::to_string(i);
        }
        break;
    }
    case 1:
    {
        constexpr size_t language_count = static_cast<size_t>(Locale::Language::COUNT);
        for (size_t i = 0; i < language_count; ++i)
        {
            if (i > 0)
            {
                options_text += ';';
            }
            options_text += Locale::kLanguageNames[i];
        }
        break;
    }
    case 2:
    {
        constexpr size_t num_style_count = sizeof(Display::kNumStyles) / sizeof(Display::kNumStyles[0]);
        for (size_t i = 0; i < num_style_count; ++i)
        {
            if (i > 0)
            {
                options_text += ';';
            }
            options_text += Display::kNumStyles[i];
        }
        break;
    }
    default:
        SendResponse(Command::GET_CONFIG_OPTION_VALUES, Status::INVALID_DATA);
        return;
    }

    if (options_text.size() > MAX_MESSAGE_LENGTH)
    {
        options_text.resize(MAX_MESSAGE_LENGTH);
    }

    SendResponse(Command::GET_CONFIG_OPTION_VALUES,
                 Status::OK,
                 reinterpret_cast<const uint8_t *>(options_text.data()),
                 static_cast<uint8_t>(options_text.size()));
}

void Communication::HandleGetBatteryLevel()
{
    // Get battery level as 0.0-1.0 and convert to 0-100 percentage
    float battery_level_float = App::battery.GetLevel();
    uint8_t battery_level_percent = static_cast<uint8_t>(battery_level_float * 100.0f);

    // Clamp to 0-100 range
    if (battery_level_percent > 100)
    {
        battery_level_percent = 100;
    }

    SendResponse(Command::GET_BATTERY_LEVEL, Status::OK,
                 &battery_level_percent, sizeof(uint8_t));
}

void Communication::HandleDisplayTime()
{
    // Force display the time
    App::ChangeLayer(Layer::Type::NORMAL);

    SendResponse(Command::DISPLAY_TIME, Status::OK);
}

void Communication::HandleGetVersion()
{
    const char *version = App::kVersion;
    size_t version_len = std::strlen(version);

    if (version_len > MAX_MESSAGE_LENGTH)
    {
        version_len = MAX_MESSAGE_LENGTH;
    }

    SendResponse(Command::GET_VERSION, Status::OK,
                 reinterpret_cast<const uint8_t *>(version),
                 static_cast<uint8_t>(version_len));
}

void Communication::ResetReceiveBuffer()
{
    rx_index_ = 0;
    message_ready_ = false;
}

void Communication::HandleDisplayIntro()
{
    App::TriggerIntro();
    SendResponse(Command::DISPLAY_INTRO, Status::OK);
}