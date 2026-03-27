/*
MIT License

Copyright (c) 2026 Vaclav Mach (xx0x)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "dev/AdcInput.hpp"
#include "dev/PwmBrightness.hpp"
#include "dev/ShiftRegister.hpp"
#include "utils/EnumTools.hpp"

/**
 * @brief Controls LED clock display content and brightness.
 * @author Vaclav Mach (xx0x)
 * @date 2026-02-02
 */
class Display
{
public:
    /**
     * @brief Maximum brightness value accepted by the display.
     */
    static constexpr uint16_t kMaxBrightness = PwmBrightness::kResolution;

    /**
     * @brief Maps logical LED identifiers to actual shift-register bit positions.
     * @warning Don't change the order unless you know what you're doing.
     */
    enum class Led : size_t
    {
        RIGHT = 0u,
        UP = 1u,
        LEFT = 2u,
        DOWN = 3u,
        PM = 4u,
        MINUTES = 8u,
        HOURS = 9u,
        AFTER = 10u,
        BEFORE = 11u,
        NUM_12 = 12u,
        NUM_11 = 13u,
        NUM_10 = 14u,
        NUM_9 = 15u,
        NUM_8 = 16u,
        NUM_7 = 17u,
        NUM_6 = 18u,
        NUM_5 = 19u,
        NUM_1 = 20u,
        NUM_2 = 21u,
        NUM_3 = 22u,
        NUM_4 = 23u
    };

    /**
     * @brief Represents the "plus" shaped clock face orientation.
     */
    enum class ClockFace
    {
        UP = 0,
        RIGHT = 1,
        DOWN = 2,
        LEFT = 3,
        COUNT
    };

    /**
     * @brief Lists top color indicator LEDs.
     */
    enum class TopLed
    {
        BEFORE,  ///< RED
        AFTER,   ///< GREEN
        HOURS,   ///< BLUE
        MINUTES, ///< YELLOW
        COUNT
    };

    /**
     * @brief Lists supported visual styles for number rendering.
     */
    enum class NumStyle : uint8_t
    {
        BAR_REVERSED, ///< OG behavior
        BAR,
        SINGLE_REVERSED,
        SINGLE,
        SNAKE_REVERSED, ///< Shows 3 LEDs in a row for snake effect (not ideal for specific numbers)
        SNAKE,          ///< Shows 3 LEDs in a row for snake effect (not ideal for specific numbers)
        COUNT
    };

    /**
     * @brief Human-readable names for selectable number styles. Used by the Web App.
     */
    static constexpr const char *kNumStyles[4] = {
        "Bar reversed (default)",
        "Bar",
        "Single reversed",
        "Single",
    };

    /**
     * @brief Initializes display hardware and internal state.
     */
    void Init();

    /**
     * @brief Deinitializes display hardware.
     */
    void DeInit();

    /**
     * @brief Triggers automatic brightness update from ambient light.
     */
    void TriggerAutoBrightness();

    /**
     * @brief Clears all display LEDs from the output buffer.
     */
    void Clear();

    /**
     * @brief Flushes output buffer to the physical display.
     */
    void Update();

    /**
     * @brief Sets active clock face orientation.
     * @param face Clock face orientation value.
     */
    void SetClockFace(ClockFace face);

    /**
     * @brief Lights up PM indicator LED.
     * @param on True to light up PM indicator.
     */
    void SetPm(bool on);

    /**
     * @brief Lights up one of the top indicator LEDs.
     * @param led Top indicator to activate.
     */
    void SetTopLed(TopLed led);

    /**
     * @brief Displays a number using selected style.
     * @param number Number to display.
     * @param style Number rendering style.
     */
    void SetNumber(size_t number, NumStyle style = NumStyle::COUNT);

    /**
     * @brief Sets individual LED state in output buffer.
     * @param led LED identifier.
     * @param on True to turn LED on.
     */
    void SetLed(Led led, bool on);

    /**
     * @brief Runs display test procedure.
     */
    void TestProcedure();

    /**
     * @brief Total number of LEDs controlled by the display.
     */
    static constexpr size_t kLedCount = 24;

    /**
     * @brief Number of LEDs used for numbers.
     */
    static constexpr size_t kNumLedCount = 12;

private:
    /**
     * @brief Controls display power enable pin.
     * @param on True to power display circuitry.
     */
    void SetPower(bool on);

    static constexpr Pin kDisplayEnablePin = {GPIOA, GPIO_PIN_1};

    std::bitset<kLedCount> data_;
    ShiftRegister<kLedCount> display_register_;
    PwmBrightness brightness_;
    AdcInput ambient_light_;

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
