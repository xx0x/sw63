#pragma once

#include "dev/AdcInput.hpp"
#include "dev/PwmBrightness.hpp"
#include "dev/ShiftRegister.hpp"
#include "utils/EnumTools.hpp"

class Display
{
public:
    static constexpr uint16_t kMaxBrightness = PwmBrightness::kResolution;

    // Change this based on the actual LED wiring
    enum class Led : size_t
    {
        LEFT = 0u,
        DOWN = 1u,
        RIGHT = 2u,
        UP = 3u,
        PM = 4u,
        BEFORE = 8u,
        AFTER = 9u,
        HOURS = 10u,
        MINUTES = 11u,
        NUM_12 = 12u,
        NUM_11 = 13u,
        NUM_10 = 14u,
        NUM_9 = 15u,
        NUM_1 = 16u,
        NUM_2 = 17u,
        NUM_3 = 18u,
        NUM_4 = 19u,
        NUM_8 = 20u,
        NUM_7 = 21u,
        NUM_6 = 22u,
        NUM_5 = 23u,
    };

    enum class ClockFace
    {
        UP,
        RIGHT,
        DOWN,
        LEFT,
        COUNT,
    };

    enum class TopLed
    {
        BEFORE,
        AFTER,
        HOURS,
        MINUTES,
        COUNT,
    };

    enum class NumStyle
    {
        SINGLE,
        SINGLE_REVERSED,
        BAR,
        BAR_REVERSED,   // OG behavior
        SNAKE,          // Shows 3 LEDs in a row for snake effect (not ideal for specific numbers)
        SNAKE_REVERSED, // Shows 3 LEDs in a row for snake effect (not ideal for specific numbers)
        COUNT
    };

    void Init();

    void DeInit();

    void TriggerAutoBrightness();

    void Clear();

    void Update();

    void SetClockFace(ClockFace face);

    void SetPm(bool on);

    void SetTopLed(TopLed led);

    void SetNumber(size_t number, NumStyle style = NumStyle::COUNT);

    void SetLed(Led led, bool on);

    void TestProcedure();

    static constexpr size_t kLedCount = 24;
    static constexpr size_t kNumLedCount = 12;

private:
    void SetPower(bool on);

    static constexpr Pin kDisplayEnablePin = {GPIOA, GPIO_PIN_1};

    std::bitset<kLedCount> data_;
    ShiftRegister<kLedCount> display_register_;
    PwmBrightness brightness_;
    AdcInput ambient_light_;
    NumStyle num_style_ = NumStyle::BAR_REVERSED;

    // Lookup tables for different modes
    // Don't change the order of these arrays.
    // Change the enum Led above if needed.
    static constexpr Led kNumLeds[kNumLedCount] = {
        Led::NUM_1, Led::NUM_2, Led::NUM_3, Led::NUM_4, Led::NUM_5, Led::NUM_6,
        Led::NUM_7, Led::NUM_8, Led::NUM_9, Led::NUM_10, Led::NUM_11, Led::NUM_12};

    static constexpr Led kRevNumLeds[kNumLedCount] = {
        Led::NUM_12, Led::NUM_11, Led::NUM_10, Led::NUM_9, Led::NUM_8, Led::NUM_7,
        Led::NUM_6, Led::NUM_5, Led::NUM_4, Led::NUM_3, Led::NUM_2, Led::NUM_1};
};
