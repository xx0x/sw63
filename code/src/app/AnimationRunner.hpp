#pragma once

#include "LocaleConfig.hpp"
#include "animations/Animation.hpp"
#include "animations/AnimationCharge.hpp"
#include "animations/AnimationIntro.hpp"
#include "animations/AnimationTime.hpp"
#include "dev/Display.hpp"
#include "dev/System.hpp"
#include "stm32l0xx_hal.h"
#include "utils/EnumTools.hpp"
#include <array>
#include <memory>

class AnimationRunner
{
public:
    enum class AnimationType
    {
        INTRO,
        TIME,
        CHARGE,
        COUNT
    };

    AnimationRunner()
    {
        animations_[AnimationType::INTRO] = std::make_unique<AnimationIntro>();
        animations_[AnimationType::TIME] = std::make_unique<AnimationTime>();
        animations_[AnimationType::CHARGE] = std::make_unique<AnimationCharge>();
    }

    void SetAnimation(AnimationType type)
    {
        current_type_ = type;
        if (auto &animation = GetCurrentAnimation())
        {
            animation->Reset();
        }
        // Initialize timing for new animation
        animation_delay_remaining_ = 0;
        last_animation_update_ = System::Millis();
    }

    void SetAnimation(AnimationType type, const LocaleConfig::TimeParameters &time_params)
    {
        SetAnimation(type);
        if (auto &animation = GetCurrentAnimation())
        {
            animation->SetTime(time_params);
        }
    }

    AnimationType GetAnimationType() const
    {
        return current_type_;
    }

    // Updates animation and returns true if continuing, false if done
    bool Update();

    void Cancel()
    {
        current_type_ = AnimationType::COUNT;
    }

private:
    AnimationType current_type_ = AnimationType::COUNT;
    EnumArray<AnimationType, std::unique_ptr<Animation>> animations_;

    // Animation timing state
    uint32_t animation_delay_remaining_ = 0;
    uint32_t last_animation_update_ = 0;

    std::unique_ptr<Animation> &GetCurrentAnimation()
    {
        return animations_[current_type_];
    }

    const std::unique_ptr<Animation> &GetCurrentAnimation() const
    {
        return animations_[current_type_];
    }

    // Returns delay in milliseconds until next frame, or 0 if finished
    uint32_t ProcessNextFrame()
    {
        if (current_type_ == AnimationType::COUNT)
        {
            return 0;
        }
        if (auto &animation = GetCurrentAnimation())
        {
            return animation->ProcessNextFrame();
        }
        return 0;
    }

    bool IsFinished() const
    {
        if (current_type_ == AnimationType::COUNT)
        {
            return true;
        }
        if (auto &animation = GetCurrentAnimation())
        {
            return animation->IsFinished();
        }
        return true;
    }

    bool ShouldPauseBetweenFrames() const
    {
        if (current_type_ == AnimationType::COUNT)
        {
            return false;
        }
        if (auto &animation = GetCurrentAnimation())
        {
            return animation->ShouldPauseBetweenFrames();
        }
        return true;
    }

    AnimationType GetCurrentAnimationType() const
    {
        return current_type_;
    }
};