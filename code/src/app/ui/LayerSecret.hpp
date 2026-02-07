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
        COMPRESSED,
        SAVER,
        EXIT,
        COUNT
    };

    Settings current_setting_ = Settings::SPEED;
    bool setting_opened_ = false;

    void ShortPressAction();
};