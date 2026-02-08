#include "AnimationRunner.hpp"
#include "App.hpp"
#include "animations/AnimationCharge.hpp"
#include "animations/AnimationIntro.hpp"
#include "animations/AnimationTime.hpp"
#include "dev/System.hpp"

AnimationRunner::AnimationRunner()
{
    animations_[AnimationType::INTRO] = std::make_unique<AnimationIntro>();
    animations_[AnimationType::TIME] = std::make_unique<AnimationTime>();
    animations_[AnimationType::CHARGE] = std::make_unique<AnimationCharge>();
}

void AnimationRunner::SetAnimation(AnimationType type)
{
    App::display.TriggerAutoBrightness();
    prev_type_ = current_type_;
    current_type_ = type;
    if (auto &animation = GetCurrentAnimation())
    {
        animation->Reset();
    }
    // Initialize timing for new animation
    animation_delay_remaining_ = 0;
    last_animation_update_ = System::Millis();
}

void AnimationRunner::SetAnimation(AnimationType type, const Locale::TimeParameters &time_params)
{
    SetAnimation(type);
    if (auto &animation = GetCurrentAnimation())
    {
        animation->SetTime(time_params);
    }
}

std::unique_ptr<Animation> &AnimationRunner::GetCurrentAnimation()
{
    return animations_[current_type_];
}

const std::unique_ptr<Animation> &AnimationRunner::GetCurrentAnimation() const
{
    return animations_[current_type_];
}

uint32_t AnimationRunner::ProcessNextFrame()
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

bool AnimationRunner::IsFinished() const
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

bool AnimationRunner::ShouldPauseBetweenFrames() const
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

bool AnimationRunner::Update()
{
    // If no animation is running, nothing to do
    if (IsFinished())
    {
        prev_type_ = current_type_;
        current_type_ = AnimationType::COUNT;
        return false;
    }

    uint32_t current_time = System::Millis();

    // Check if it's time for the next animation frame
    if (current_time >= last_animation_update_ + animation_delay_remaining_)
    {
        App::display.Clear();
        App::display.Update();
        // Handle pause between frames for time display animations
        if (ShouldPauseBetweenFrames())
        {
            System::Delay(App::settings.GetSpeedOption().pause);
        }
        // Update display brightness based on ambient light (the lights need to be off to read it properly)
        App::display.TriggerAutoBrightness();

        // Process the next frame and get the delay until the next update
        animation_delay_remaining_ = ProcessNextFrame();
        last_animation_update_ = System::Millis();

        // If the animation finished, clear the display
        if (IsFinished())
        {
            App::display.Clear();
            App::display.Update();
            prev_type_ = current_type_;
            current_type_ = AnimationType::COUNT;
            return false; // Animation is done
        }
    }

    return true; // Animation is still running
}