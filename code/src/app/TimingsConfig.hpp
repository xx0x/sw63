#pragma once

#include <array>
#include <cstdint>

class TimingsConfig
{
public:
    struct Speed
    {
        uint32_t hold;
        uint32_t hold_digits;
        uint32_t pause;
    };

    void SetSpeed(uint32_t speed)
    {
        if (speed < kSpeedCount)
        {
            speed_index_ = speed;
        }
    }

    void SetSpeedOverride(size_t speed_index_override)
    {
        speed_index_override_ = speed_index_override;
    }

    void ClearSpeedOverride()
    {
        speed_index_override_ = std::nullopt;
    }

    Speed GetSpeed() const
    {
        if (speed_index_override_.has_value())
        {
            return kSpeeds[speed_index_override_.value()];
        }
        return kSpeeds[speed_index_];
    }

    static constexpr size_t GetSpeedCount()
    {
        return kSpeedCount;
    }

private:
    // Speed configuration arrays (slowest to fastest)
    static constexpr std::array kSpeeds = {
        Speed{.hold = 1000, .hold_digits = 1000, .pause = 150},
        Speed{.hold = 800, .hold_digits = 800, .pause = 120},
        Speed{.hold = 600, .hold_digits = 600, .pause = 90},
        Speed{.hold = 400, .hold_digits = 400, .pause = 60},
        Speed{.hold = 300, .hold_digits = 300, .pause = 45},
        Speed{.hold = 150, .hold_digits = 150, .pause = 23}};

    static constexpr size_t kSpeedCount = kSpeeds.size();
    size_t speed_index_ = 0;
    std::optional<size_t> speed_index_override_ = std::nullopt;
};