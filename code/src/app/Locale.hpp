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

#include "animations/ClockFrame.hpp"
#include "dev/Display.hpp"

/**
 * @brief Converts raw time values into language-specific display frames.
 * @author Vaclav Mach (xx0x)
 * @date 2026-02-02
 */
class Locale
{
public:
    /**
     * @brief Supported languages for the clock.
     * @details When adding a new language, ensure to update Locale.cpp:
     *          - GetTimeCoefficients() with appropriate coefficients
     *          - GetSequence() with the correct frame sequences
     *          - ProcessTime() if any special handling is needed (hopefully not needed)
     */
    enum class Language : uint8_t
    {
        CZECH, // Also works for: SLOVAK, GERMAN (EAST)
        ENGLISH,
        GERMAN, // Also works for: DUTCH
        POLISH,
        HUNGARIAN,
        NORDIC, // Also works for: NORWEGIAN, DANISH, SWEDISH, FINNISH
        COUNT
    };

    /**
     * @brief     Human-readable names for the supported languages.
     * @details   This array needs to be in the same order as the Language enum.
     */
    static constexpr const char *kLanguageNames[static_cast<size_t>(Language::COUNT)] = {
        "Czech, Slovak, German (East)",
        "English",
        "German, Dutch",
        "Polish",
        "Hungarian",
        "Nordic"};

    /**
     * @brief Each language has its' own coefficients for quarter/half/to adjustments.
     */
    struct TimeCoefficients
    {
        uint32_t first_quarter; // how "quarter after/quarter of" modifies hours
        uint32_t half;          // how "half past/half of" modifies hours
        uint32_t third_quarter; // how "quarter to/three quarters of" modifies hours
    };

    /**
     * @brief Types of time expressions passed back and forth.
     */
    enum class TimeType
    {
        EXACT,
        PAST,
        TO,
        COUNT
    };

    /**
     * @brief Holds language-processed time values used by animations.
     */
    struct TimeParameters
    {
        TimeType time_type;
        uint32_t hours;
        uint32_t minutes;
        Display::ClockFace face;
        bool pm;
    };

    /**
     * @brief     Get the sequence of frames for the given time type and language.
     * @param     time_type  The type of time (EXACT, PAST, TO).
     * @return    A FrameSequence containing the frames to display.
     */
    FrameSequence GetSequence(TimeType time_type) const;

    /**
     * @brief     Process the hh:mm time to "human-like" time parameters.
     * @details   This function takes hours and minutes, and returns the appropriate
     *            TimeParameters for the current language.
     * @param     hours  Hours in 24-hour format.
     * @param     minutes  Minutes in 60-minute format.
     * @return    Processed TimeParameters structure.
     */
    TimeParameters ProcessTime(const uint32_t hours, const uint32_t minutes) const;

    /**
     * @brief     Ensure hours are in the correct range the display can show (1-12).
     * @details   The hours can be 0-12
     *            Normally would be 0-11 but there are coefficients applied for some languages
     * @param     params  Input TimeParameters structure.
     * @return    Fixed TimeParameters structure.
     */
    TimeParameters FixTime(const TimeParameters &params) const;

    /**
     * @brief Returns whether the hour should be decremented based on the clock face and minutes offset (based on the language rules).
     * @param face The clock face position (UP, RIGHT, DOWN, LEFT).
     * @param minutes The minutes in the actual hour.
     * @return Hours offset
     */
    int32_t HourOffsetWhenSaving(Display::ClockFace face, int minutes) const;

private:
    /**
     * @brief     Get the time coefficients for the current language.
     * @return    TimeCoefficients structure with the coefficients.
     */
    TimeCoefficients GetTimeCoefficients() const;
};