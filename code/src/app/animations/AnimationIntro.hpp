#pragma once

#include "Animation.hpp"

class AnimationIntro : public Animation
{
public:
    uint32_t ProcessNextFrame() override;
};