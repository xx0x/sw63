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

#include <cstdint>
#include <vector>

/**
 * @brief Animation frame elements rendered on the clock display.
 */
enum class ClockFrame
{
    MINUTES_NUM, // Display the minutes number
    MINUTES,     // "minutes" indicator LED
    BEFORE,      // "before/to" indicator LED
    AFTER,       // "after/past" indicator LED
    CLOCKFACE,   // Clock face position (UP, DOWN, LEFT, RIGHT)
    HOURS_NUM,   // Display the hours number
    HOURS,       // "hours" indicator LED
    PM,          // PM indicator
    COUNT
};

/**
 * @brief Helper type container for clock frame elements.
 */
using FrameSequence = std::vector<ClockFrame>;