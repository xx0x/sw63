#pragma once

#include <cstdint>

/**
 * @brief Maps a value from one range to another.
 * @param value The input value to map.
 * @param in_min The minimum of the input range.
 * @param in_max The maximum of the input range.
 * @param out_min The minimum of the output range.
 * @param out_max The maximum of the output range.
 * @return The mapped value in the output range.
 */
static constexpr int32_t map(int32_t value, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**
 * @brief Maps a value from one range to another.
 * @param value The input value to map.
 * @param in_min The minimum of the input range.
 * @param in_max The maximum of the input range.
 * @param out_min The minimum of the output range.
 * @param out_max The maximum of the output range.
 * @return The mapped value in the output range.
 */
static constexpr float mapf(float value, float in_min, float in_max, float out_min, float out_max)
{
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}