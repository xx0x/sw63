#pragma once

#include "../Animation.hpp"
#include "../TimingsConfig.hpp"

class AnimationIntro : public Animation
{
public:
    uint32_t ProcessNextFrame() override;
};