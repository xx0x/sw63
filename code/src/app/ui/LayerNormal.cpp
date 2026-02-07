
#include "LayerNormal.hpp"
#include "app/App.hpp"
#include "dev/System.hpp"

void LayerNormal::OnEvent(Event event)
{
    switch (event)
    {
    case Event::INIT:
        IntroAnimation();
        break;
    case Event::ENTER:
        DisplayTime();
        break;
    case Event::JUST_PRESSED:
        sleep_allowed_ = false;
        if (animation_runner_.GetAnimationType() != AnimationRunner::AnimationType::INTRO)
        {
            DisplayTime();
        }
        break;
    case Event::MEDIUM_PRESS:
        App::ChangeLayer(Layer::Type::SETTINGS);
        break;
    case Event::MULTI_PRESS:
        IntroAnimation();
        break;
    case Event::LEAVE:
        animation_runner_.Cancel();
        break;
    case Event::JUST_RELEASED:
    case Event::LONG_PRESS:
    case Event::COUNT:
    default:
        break;
    }
}

void LayerNormal::Update()
{
    // Process animations
    if (!animation_runner_.Update())
    {
        // Animation just finished - check what type it was
        AnimationRunner::AnimationType finished_type = animation_runner_.GetPreviousAnimationType();

        if (finished_type == AnimationRunner::AnimationType::INTRO)
        {
            // INTRO animation finished...
            // Short delay after intro before showing time
            System::Delay(500);
            DisplayTime();
        }
        else if (System::GetRawUsbPowerState())
        {
            animation_runner_.SetAnimation(AnimationRunner::AnimationType::CHARGE);
        }
        else
        {
            sleep_allowed_ = true;
        }
    }
}

bool LayerNormal::SleepAllowed()
{
    return sleep_allowed_;
}

void LayerNormal::DisplayTime()
{
    auto now = App::rtc.GetDateTime();
    if (!now)
    {
        return; // Failed to get time
    }

    // Use LocaleConfig to process the time and determine animation parameters
    auto time_params = App::locale.ProcessTime(now->hour, now->minute);

    // Set the time animation with the processed parameters
    animation_runner_.SetAnimation(AnimationRunner::AnimationType::TIME, time_params);
}

void LayerNormal::IntroAnimation()
{
    animation_runner_.SetAnimation(AnimationRunner::AnimationType::INTRO);
}