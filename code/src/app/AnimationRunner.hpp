#pragma once

#include "LocaleConfig.hpp"
#include "animations/Animation.hpp"
#include "utils/EnumTools.hpp"
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

    AnimationRunner();

    void SetAnimation(AnimationType type);

    void SetAnimation(AnimationType type, const LocaleConfig::TimeParameters &time_params);

    AnimationType GetAnimationType() const
    {
        return current_type_;
    }

    // Updates animation and returns true if continuing, false if done
    bool Update();

    void Cancel()
    {
        prev_type_ = current_type_;
        current_type_ = AnimationType::COUNT;

    }

    AnimationType GetPreviousAnimationType() const
    {
        return prev_type_;
    }

private:
    AnimationType current_type_ = AnimationType::COUNT;
    AnimationType prev_type_ = AnimationType::COUNT;
    EnumArray<AnimationType, std::unique_ptr<Animation>> animations_;

    // Animation timing state
    uint32_t animation_delay_remaining_ = 0;
    uint32_t last_animation_update_ = 0;

    std::unique_ptr<Animation> &GetCurrentAnimation();
    const std::unique_ptr<Animation> &GetCurrentAnimation() const;

    // Returns delay in milliseconds until next frame, or 0 if finished
    uint32_t ProcessNextFrame();
    bool IsFinished() const;
    bool ShouldPauseBetweenFrames() const;
};