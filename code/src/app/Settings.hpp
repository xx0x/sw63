#pragma once

#include "Locale.hpp"
#include "dev/Display.hpp"
#include <array>
#include <cstddef>
#include <cstdint>

class Settings
{
public:
    struct Config
    {
        uint8_t speed = kDefaultSpeed;
        Locale::Language language = kDefaultLanguage;
        Display::NumStyle num_style = kDefaultNumStyle;
    };

    struct SpeedOption
    {
        uint32_t hold;
        uint32_t hold_digits;
        uint32_t pause;
    };

    Config GetConfig() const
    {
        return config_;
    }

    Config &GetConfig()
    {
        return config_;
    }

    void SetConfig(const Config &config)
    {
        config_ = config;
    }

    void SetLanguage(Locale::Language language)
    {
        config_.language = language;
    }

    Locale::Language GetLanguage() const
    {
        return config_.language;
    }

    void SetSpeed(uint32_t speed)
    {
        if (speed < kSpeedOptionsCount)
        {
            config_.speed = speed;
        }
    }

    int32_t GetSpeed() const
    {
        return config_.speed;
    }

    SpeedOption GetSpeedOption() const
    {
        return kSpeedOptions[config_.speed];
    }

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

    static constexpr Display::NumStyle kDefaultNumStyle = Display::NumStyle::BAR_REVERSED;

    static constexpr uint32_t kInactivityTimeoutMs = 15000;
    static constexpr float kLowBatteryThreshold = 0.1f;

private:
    // Speed configuration arrays (slowest to fastest)
    static constexpr std::array kSpeedOptions = {
        SpeedOption{.hold = 1000, .hold_digits = 1000, .pause = 150},
        SpeedOption{.hold = 800, .hold_digits = 800, .pause = 120},
        SpeedOption{.hold = 600, .hold_digits = 600, .pause = 90},
        SpeedOption{.hold = 400, .hold_digits = 400, .pause = 60},
        SpeedOption{.hold = 300, .hold_digits = 300, .pause = 45},
        SpeedOption{.hold = 150, .hold_digits = 150, .pause = 23}};

    static constexpr size_t kSpeedOptionsCount = kSpeedOptions.size();
    Config config_;
};