#pragma once

#include "Layer.hpp"
#include "app/LocaleConfig.hpp"
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

    struct TimeSetting
    {
        Display::ClockFace face = Display::ClockFace::UP;
        int32_t hours = 1;
        int32_t minutes = 1;
        LocaleConfig::TimeType past_to = LocaleConfig::TimeType::PAST;
        bool pm = true;
    };

    Step settings_step_ = Step::LANGUAGE;
    TimeSetting time_setting_;

    void StoreSettings();
    void ShortPressAction();
};