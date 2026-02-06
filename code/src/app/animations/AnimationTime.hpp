#pragma once

#include "../Animation.hpp"
#include "../LocaleConfig.hpp"
#include "../TimingsConfig.hpp"
#include "ClockFrame.hpp"

class AnimationTime : public Animation
{
public:
    AnimationTime() = default;
    uint32_t ProcessNextFrame() override;
    void Reset() override;

    bool ShouldPauseBetweenFrames() const override
    {
        return true;
    }

private:
    FrameSequence sequence_;
    uint32_t ProcessFrameElement(ClockFrame frame);
};