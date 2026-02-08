#include "Communication.hpp"
#include "app/App.hpp"
#include "dev/System.hpp"
#include "tusb.h"
#include <cstring>

bool Communication::Init()
{
    // Initialize TinyUSB
    if (!tusb_init()) {
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
    if (!tud_cdc_connected()) {
        return;
    }
    
    // Read available data
    while (tud_cdc_available()) {
        uint8_t byte = tud_cdc_read_char();
        
        // Add byte to buffer
        if (rx_index_ < rx_buffer_.size()) {
            rx_buffer_[rx_index_++] = byte;
            
            // Check if we have a complete message
            if (rx_index_ >= HEADER_SIZE) {
                uint8_t expected_length = rx_buffer_[1];
                if (rx_index_ >= HEADER_SIZE + static_cast<size_t>(expected_length)) {
                    message_ready_ = true;
                }
            }
        } else {
            // Buffer overflow, reset
            ResetReceiveBuffer();
        }
    }
    
    // Process complete message if ready
    if (message_ready_) {
        Command command = static_cast<Command>(rx_buffer_[0]);
        uint8_t length = rx_buffer_[1];
        const uint8_t* data = &rx_buffer_[HEADER_SIZE];
        
        HandleMessage(command, data, length);
        ResetReceiveBuffer();
    }
}

bool Communication::IsConnected() const
{
    return tud_cdc_connected();
}

void Communication::HandleMessage(Command command, const uint8_t* data, uint8_t length)
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
        case Command::GET_BATTERY_LEVEL:
            HandleGetBatteryLevel();
            break;
        default:
            SendResponse(command, Status::INVALID_COMMAND);
            break;
    }
}

void Communication::SendResponse(Command command, Status status, const uint8_t* data, uint8_t length)
{
    // Create response packet: [Command][Status][Length][Data...]
    std::array<uint8_t, MAX_PACKET_SIZE> response;
    response[0] = static_cast<uint8_t>(command);
    response[1] = static_cast<uint8_t>(status);
    response[2] = length;
    
    if (data && length > 0) {
        std::memcpy(&response[3], data, length);
    }
    
    size_t total_length = 3 + length;
    
    // Send response via USB CDC
    tud_cdc_write(&response[0], total_length);
    tud_cdc_write_flush();
}

void Communication::HandleSetTime(const uint8_t* data, uint8_t length)
{
    if (length != sizeof(DS3231::DateTime)) {
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
        datetime.year < 2000 || datetime.year > 2099) {
        SendResponse(Command::SET_TIME, Status::INVALID_DATA);
        return;
    }
    
    // Set the time on the RTC
    bool success = App::rtc.SetDateTime(datetime);
    
    if (success) {
        SendResponse(Command::SET_TIME, Status::OK);
    } else {
        SendResponse(Command::SET_TIME, Status::ERROR);
    }
}

void Communication::HandleGetTime()
{
    auto datetime_opt = App::rtc.GetDateTime();
    
    if (datetime_opt.has_value()) {
        DS3231::DateTime datetime = datetime_opt.value();
        SendResponse(Command::GET_TIME, Status::OK, 
                    reinterpret_cast<const uint8_t*>(&datetime), 
                    sizeof(DS3231::DateTime));
    } else {
        SendResponse(Command::GET_TIME, Status::ERROR);
    }
}

void Communication::HandleSetConfig(const uint8_t* data, uint8_t length)
{
    if (length != sizeof(Settings::Config)) {
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
                reinterpret_cast<const uint8_t*>(&config), 
                sizeof(Settings::Config));
}

void Communication::HandleGetBatteryLevel()
{
    // Get battery level as 0.0-1.0 and convert to 0-100 percentage
    float battery_level_float = App::battery.GetLevel();
    uint8_t battery_level_percent = static_cast<uint8_t>(battery_level_float * 100.0f);
    
    // Clamp to 0-100 range
    if (battery_level_percent > 100) {
        battery_level_percent = 100;
    }
    
    SendResponse(Command::GET_BATTERY_LEVEL, Status::OK, 
                &battery_level_percent, sizeof(uint8_t));
}

void Communication::ResetReceiveBuffer()
{
    rx_index_ = 0;
    message_ready_ = false;
}