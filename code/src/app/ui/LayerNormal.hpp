#pragma once

#include "Layer.hpp"
#include "app/AnimationRunner.hpp"

class LayerNormal : public Layer
{
public:
    void OnEvent(Event event) override;
    void Update() override;
    bool SleepAllowed() override;

private:
    void IntroAnimation();
    void DisplayTime();

    bool sleep_allowed_ = false;

    AnimationRunner animation_runner_;
};