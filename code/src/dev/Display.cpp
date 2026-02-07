#include "Display.hpp"
#include "utils/math_utils.hpp"
#include <algorithm>

void Display::Init()
{
    // Enable display power
    SetPower(true);

    // Initialize PWM brightness control
    brightness_.Init();

    // Initialize ambient light sensor (PA2)
    ambient_light_.Init({.port = GPIOA,
                         .pin = GPIO_PIN_2,
                         .channel = ADC_CHANNEL_2});

    // Set initial brightness based on ambient light
    TriggerAutoBrightness();

    // Shift register for LEDs
    display_register_.Init({
        .data = {GPIOA, GPIO_PIN_7},  // MOSI - PA7
        .clock = {GPIOA, GPIO_PIN_5}, // SCK - PA5
        .latch = {GPIOA, GPIO_PIN_8}  // LATCH - PA8
    });

    // Clear display state
    Clear();

    // Update display
    Update();
}

void Display::SetPower(bool on)
{
    if (on)
    {
        GPIO_InitTypeDef GPIO_InitStruct = {
            .Pin = kDisplayEnablePin.pin,
            .Mode = GPIO_MODE_OUTPUT_PP,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_VERY_HIGH,
            .Alternate = 0,
        };
        HAL_GPIO_Init(kDisplayEnablePin.port, &GPIO_InitStruct);
        HAL_GPIO_WritePin(kDisplayEnablePin.port, kDisplayEnablePin.pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_DeInit(kDisplayEnablePin.port, kDisplayEnablePin.pin);
    }
}

void Display::DeInit()
{
    Clear();
    Update();
    brightness_.DeInit();
    ambient_light_.DeInit();
    display_register_.DeInit();
    SetPower(false);
}

void Display::TriggerAutoBrightness()
{
    // Read ambient light from PA02 (ADC channel 2)
    auto ambient_result = ambient_light_.ReadSingleShot();
    if (!ambient_result.has_value())
    {
        // ADC read failed, use default brightness
        brightness_.Set(kMaxBrightness / 2);
        return;
    }
    uint16_t ambient_reading = ambient_result.value();

    // Map the ADC value (0-4095) to brightness range (1-1023)
    // We want some brightness even if there's no light
    uint16_t mapped_brightness = map(ambient_reading, 0, AdcInput::kResolution, 1, kMaxBrightness);

    // Set the brightness
    brightness_.Set(mapped_brightness);
}

void Display::Clear()
{
    data_.reset();
}

void Display::Update()
{
    display_register_.Write(data_);
}

void Display::SetClockFace(ClockFace face)
{
    data_.set(static_cast<size_t>(Led::UP), ClockFace::UP == face);
    data_.set(static_cast<size_t>(Led::RIGHT), ClockFace::RIGHT == face);
    data_.set(static_cast<size_t>(Led::DOWN), ClockFace::DOWN == face);
    data_.set(static_cast<size_t>(Led::LEFT), ClockFace::LEFT == face);
}

void Display::SetPm(bool on)
{
    data_.set(static_cast<size_t>(Led::PM), on);
}

void Display::SetTopLed(TopLed led)
{
    data_.set(static_cast<size_t>(Led::HOURS), TopLed::HOURS == led);
    data_.set(static_cast<size_t>(Led::MINUTES), TopLed::MINUTES == led);
    data_.set(static_cast<size_t>(Led::AFTER), TopLed::AFTER == led);
    data_.set(static_cast<size_t>(Led::BEFORE), TopLed::BEFORE == led);
}

void Display::SetNumber(size_t number, NumStyle style)
{
    if (style == NumStyle::COUNT)
    {
        style = num_style_;
    }

    for (auto led : kNumLeds)
    {
        data_.set(static_cast<size_t>(led), false);
    }
    if (number == 0 || number > kNumLedCount)
    {
        return;
    }

    switch (style)
    {
    case NumStyle::SINGLE:
        data_.set(static_cast<size_t>(kNumLeds[number - 1]), true);
        break;

    case NumStyle::SINGLE_REVERSED:
        data_.set(static_cast<size_t>(kRevNumLeds[number - 1]), true);
        break;

    case NumStyle::BAR:
        for (size_t i = 0; i < number; ++i)
        {
            data_.set(static_cast<size_t>(kNumLeds[i]), true);
        }
        break;
    case NumStyle::BAR_REVERSED:
        for (size_t i = 0; i < number; ++i)
        {
            data_.set(static_cast<size_t>(kRevNumLeds[i]), true);
        }
        break;
    case NumStyle::SNAKE:
        data_.set(static_cast<size_t>(kNumLeds[number - 1]), true);
        data_.set(static_cast<size_t>(kNumLeds[number % Display::kNumLedCount]), true);
        data_.set(static_cast<size_t>(kNumLeds[(number + 1) % Display::kNumLedCount]), true);
        break;
    case NumStyle::SNAKE_REVERSED:
        data_.set(static_cast<size_t>(kRevNumLeds[number - 1]), true);
        data_.set(static_cast<size_t>(kRevNumLeds[number % Display::kNumLedCount]), true);
        data_.set(static_cast<size_t>(kRevNumLeds[(number + 1) % Display::kNumLedCount]), true);
        break;
    case NumStyle::COUNT:
    default:
        break;
    }
}

void Display::SetLed(Led led, bool on)
{
    data_.set(static_cast<size_t>(led), on);
}

void Display::TestProcedure()
{
    // Just for the development!!!
    // The intro animation is something different
    for (int i = 0; i <= 12; i++)
    {
        Clear();
        SetNumber(i);
        Update();
        HAL_Delay(200);
    }

    for (auto top_led : EnumRange<Display::TopLed>())
    {
        Clear();
        SetTopLed(top_led);
        Update();
        HAL_Delay(200);
    }

    for (auto clockface : EnumRange<Display::ClockFace>())
    {
        Clear();
        SetClockFace(clockface);
        Update();
        HAL_Delay(200);
    }

    for (int i = 0; i < 3; i++)
    {
        Clear();
        SetPm(true);
        Update();
        HAL_Delay(200);
        SetPm(false);
        Update();
        HAL_Delay(200);
    }
}