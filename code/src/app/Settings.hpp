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

#include "Locale.hpp"
#include "dev/Display.hpp"
#include <array>
#include <cstddef>
#include <cstdint>

/**
 * @brief Stores and validates runtime watch configuration.
 * @author Vaclav Mach (xx0x)
 * @date 2026-02-08
 */
class Settings
{
public:
    /**
     * @brief Configuration structure for watch settings
     * @note This structure is packed to ensure consistent memory layout
     *       for serial communication. Total size: 3 bytes (3×uint8_t)
     *       without padding for proper protocol compatibility.
     */
    struct __attribute__((packed)) Config
    {
        /** @brief Selected animation speed index. */
        uint8_t speed = kDefaultSpeed;
        /** @brief Selected display language. */
        Locale::Language language = kDefaultLanguage;
        /** @brief Selected number rendering style. */
        Display::NumStyle num_style = kDefaultNumStyle;
    };

    /**
     * @brief Stores per-speed timing parameters for animation pacing.
     */
    struct SpeedOption
    {
        uint32_t hold;
        uint32_t hold_digits;
        uint32_t pause;
    };

    /**
     * @brief Returns current configuration.
     * @return Copy of active configuration.
     */
    Config GetConfig() const
    {
        return config_;
    }

    /**
     * @brief Returns mutable access to current configuration.
     * @return Reference to active configuration.
     */
    Config &GetConfig()
    {
        return config_;
    }

    /**
     * @brief Replaces current configuration.
     * @param config New configuration values.
     */
    void SetConfig(const Config &config)
    {
        config_ = config;
    }

    /**
     * @brief Sets active display language.
     * @param language Language to store.
     */
    void SetLanguage(Locale::Language language)
    {
        config_.language = language;
    }

    /**
     * @brief Returns active display language.
     * @return Current language value.
     */
    Locale::Language GetLanguage() const
    {
        return config_.language;
    }

    /**
     * @brief Sets animation speed index when in valid range.
     * @param speed Speed option index.
     */
    void SetSpeed(uint32_t speed)
    {
        if (speed < kSpeedOptionsCount)
        {
            config_.speed = speed;
        }
    }

    /**
     * @brief Returns current speed index.
     * @return Configured speed option index.
     */
    int32_t GetSpeed() const
    {
        return config_.speed;
    }

    /**
     * @brief Returns timing values for current speed index.
     * @return Speed option timing values.
     */
    SpeedOption GetSpeedOption() const
    {
        return kSpeedOptions[config_.speed];
    }

    /**
     * @brief Returns number of available speed options.
     * @return Total speed option count.
     */
    static constexpr size_t GetSpeedCount()
    {
        return kSpeedOptionsCount;
    }

    /**
     * @brief Default language for the clock.
     */
    static constexpr Locale::Language kDefaultLanguage = static_cast<Locale::Language>(0);

    /**
     * @brief Default to the slowest speed option.
     */
    static constexpr uint32_t kDefaultSpeed = 0;

    /**
     * @brief Default number rendering style.
     */
    static constexpr Display::NumStyle kDefaultNumStyle = Display::NumStyle::BAR_REVERSED;

    /**
     * @brief Inactivity timeout before app changes to NORMAL layer.
     */
    static constexpr uint32_t kInactivityTimeoutMs = 15000;

    /**
     * @brief Battery level threshold considered low.
     */
    static constexpr float kLowBatteryThreshold = 0.1f;

private:
    // Speed configuration arrays (slowest to fastest)
    static constexpr std::array kSpeedOptions = {
        SpeedOption{.hold = 1200, .hold_digits = 1200, .pause = 180},
        SpeedOption{.hold = 1000, .hold_digits = 1000, .pause = 150},
        SpeedOption{.hold = 800, .hold_digits = 800, .pause = 120},
        SpeedOption{.hold = 600, .hold_digits = 600, .pause = 90},
        SpeedOption{.hold = 400, .hold_digits = 400, .pause = 60},
        SpeedOption{.hold = 300, .hold_digits = 300, .pause = 45},
        SpeedOption{.hold = 225, .hold_digits = 225, .pause = 30},
        SpeedOption{.hold = 180, .hold_digits = 180, .pause = 27},
        SpeedOption{.hold = 150, .hold_digits = 150, .pause = 23}};

    static constexpr size_t kSpeedOptionsCount = kSpeedOptions.size();
    Config config_;
};