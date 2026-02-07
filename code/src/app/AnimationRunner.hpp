#pragma once

#include "Animation.hpp"
#include "LocaleConfig.hpp"
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
    AnimationRunner()
    {
        animations_[Animation::Type::INTRO] = std::make_unique<AnimationIntro>();
        animations_[Animation::Type::TIME] = std::make_unique<AnimationTime>();
        animations_[Animation::Type::CHARGE] = std::make_unique<AnimationCharge>();
    }

    void SetAnimation(Animation::Type type)
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

    void SetAnimation(Animation::Type type, const LocaleConfig::TimeParameters &time_params)
    {
        SetAnimation(type);
        if (auto &animation = GetCurrentAnimation())
        {
            animation->SetTime(time_params);
        }
    }

    Animation::Type GetAnimationType() const
    {
        return current_type_;
    }

    // Updates animation and returns true if continuing, false if done
    bool Update();

    void Cancel()
    {
        current_type_ = Animation::Type::COUNT;
    }

private:
    Animation::Type current_type_ = Animation::Type::COUNT;
    EnumArray<Animation::Type, std::unique_ptr<Animation>> animations_;

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
        if (current_type_ == Animation::Type::COUNT)
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
        if (current_type_ == Animation::Type::COUNT)
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
        if (current_type_ == Animation::Type::COUNT)
        {
            return false;
        }
        if (auto &animation = GetCurrentAnimation())
        {
            return animation->ShouldPauseBetweenFrames();
        }
        return true;
    }

    Animation::Type GetCurrentAnimationType() const
    {
        return current_type_;
    }
};