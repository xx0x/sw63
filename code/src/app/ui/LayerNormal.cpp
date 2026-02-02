
#include "LayerNormal.hpp"
#include "app/App.hpp"
#include "dev/System.hpp"

void LayerNormal::OnEvent(Event event)
{
    switch (event)
    {
    case Event::SHORT_PRESS:
        if (App::animation_runner.GetAnimationType() != Animation::Type::INTRO)
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
    case Event::CHARGE_START:
        ChargeAnimation();
        break;
    case Event::ENTER:
        DisplayTime();
        break;
    case Event::LEAVE:
        App::animation_runner.Cancel();
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
    App::animation_runner.SetAnimation(Animation::Type::TIME, time_params);
}

void LayerNormal::IntroAnimation()
{
    App::display.TriggerAutoBrightness();
    App::animation_runner.SetAnimation(Animation::Type::INTRO);
}

void LayerNormal::ChargeAnimation()
{
    App::display.TriggerAutoBrightness();
    App::animation_runner.SetAnimation(Animation::Type::CHARGE);
}