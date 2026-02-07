#pragma once

#include "Layer.hpp"
#include <cstdint>

class LayerSecret : public Layer
{
public:
    void OnEvent(Event event) override;
    void Update() override;
    bool SleepAllowed() override { return false; }

private:
    enum class Settings
    {
        SPEED,
        VISUAL_STYLE,
        BATTERY_LEVEL,
        EXIT,
        COUNT
    };

    Settings current_setting_ = Settings::SPEED;
    bool setting_opened_ = false;
    float battery_level_ = 0.f;

    void ShortPressAction();
};