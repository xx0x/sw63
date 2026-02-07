
#include "LayerNormal.hpp"
#include "app/AnimationRunner.hpp"
#include "app/App.hpp"
#include "dev/System.hpp"

void LayerNormal::OnEvent(Event event)
{
    switch (event)
    {
    case Event::JUST_PRESSED:
        if (App::animation_runner.GetAnimationType() != AnimationRunner::AnimationType::INTRO)
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
    case Event::ENTER:
        DisplayTime();
        break;
    case Event::LEAVE:
        App::animation_runner.Cancel();
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
    App::animation_runner.SetAnimation(AnimationRunner::AnimationType::TIME, time_params);
}

void LayerNormal::IntroAnimation()
{
    App::display.TriggerAutoBrightness();
    App::animation_runner.SetAnimation(AnimationRunner::AnimationType::INTRO);
}