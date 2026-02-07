#pragma once

#include "Layer.hpp"
#include "dev/Display.hpp"
#include <cstdint>

class LayerSettings : public Layer
{
public:
    void OnEvent(Event event) override;
    void Update() override;
    bool SleepAllowed() override { return false; }

private:
    enum class Step
    {
        LANGUAGE,
        FACE,
        HOURS,
        PAST_TO,
        MINUTES,
        PM,
        COUNT
    };

    enum class PastTo : int8_t
    {
        PAST = 1,
        TO = -1,
        EXACT = 0
    };

    struct TimeSetting
    {
        Display::ClockFace face = Display::ClockFace::UP;
        uint8_t hours = 1;
        uint8_t minutes = 1;
        PastTo past_to = PastTo::PAST;
        bool pm = true;
    };

    Step settings_step_ = Step::LANGUAGE;
    TimeSetting time_setting_;

    void StoreSettings();
    void ShortPressAction();
};