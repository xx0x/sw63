#pragma once

#include "Layer.hpp"

class LayerNormal : public Layer
{
public:
    void OnEvent(Event event) override;
    void Update() override;
    bool SleepAllowed() override { return true; }

private:
    void IntroAnimation();
    void DisplayTime();
};