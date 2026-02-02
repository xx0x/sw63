#pragma once

#include <cstdint>
#include <vector>

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

using FrameSequence = std::vector<ClockFrame>;