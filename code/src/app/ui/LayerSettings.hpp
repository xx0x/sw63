#pragma once

#include "Layer.hpp"
#include <cstdint>

class LayerSettings : public Layer
{
public:
    void OnEvent(Event event) override;
    void Update() override;
    bool SleepAllowed() override { return false; }

private:
    uint32_t menu_step_ = 0;
};