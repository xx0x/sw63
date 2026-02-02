#pragma once

#include "Layer.hpp"

class LayerSettings : public Layer
{
public:
    void OnEvent(Event event) override {};
    void Update() override;
    bool SleepAllowed() override { return false; }
};