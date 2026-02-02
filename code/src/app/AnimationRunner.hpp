#pragma once

#include "Animation.hpp"
#include "LocaleConfig.hpp"
#include "animations/AnimationCharge.hpp"
#include "animations/AnimationIntro.hpp"
#include "animations/AnimationTime.hpp"
#include "dev/Display.hpp"
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
    }

    void SetAnimation(Animation::Type type, const LocaleConfig::TimeParameters &time_params)
    {
        SetAnimation(type);
        if (auto &animation = GetCurrentAnimation())
        {
            animation->SetTime(time_params);
        }
    }

    // Returns delay in milliseconds until next frame, or 0 if finished
    uint32_t ProcessNextFrame()
    {
        if (auto &animation = GetCurrentAnimation())
        {
            return animation->ProcessNextFrame();
        }
        return 0;
    }

    bool IsFinished() const
    {
        if (auto &animation = GetCurrentAnimation())
        {
            return animation->IsFinished();
        }
        return true;
    }

    bool IsShowingTime() const
    {
        return current_type_ == Animation::Type::TIME;
    }

    bool ShouldPauseBetweenFrames() const
    {
        return IsShowingTime();
    }

    Animation::Type GetCurrentAnimationType() const
    {
        return current_type_;
    }

private:
    Animation::Type current_type_ = Animation::Type::INTRO;
    EnumArray<Animation::Type, std::unique_ptr<Animation>> animations_;

    std::unique_ptr<Animation> &GetCurrentAnimation()
    {
        return animations_[current_type_];
    }

    const std::unique_ptr<Animation> &GetCurrentAnimation() const
    {
        return animations_[current_type_];
    }
};