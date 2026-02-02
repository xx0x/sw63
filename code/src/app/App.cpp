#include "App.hpp"
#include "app/ui/LayerNormal.hpp"
#include "app/ui/LayerSettings.hpp"
#include "dev/System.hpp"

void App::Init()
{
    display.Init();
    battery.Init();
    auto rtc_success = rtc.Init();
    if (!rtc_success)
    {
        while (1)
        {
            display.Clear();
            display.SetPm(true); // Indicate error with PM LED
            display.Update();
            System::Delay(500);
            display.Clear();
            display.SetPm(false);
            display.Update();
            System::Delay(500);
        }
    }
    // Set test date (we don't care about the date here, just time)
    rtc.SetDateTime({0, 45, 13, 1, 18, 8, 2025});

    // Set default speed
    timings.SetSpeed(0);

    layers_[Layer::Type::NORMAL] = std::make_unique<LayerNormal>();
    layers_[Layer::Type::SETTINGS] = std::make_unique<LayerSettings>();

    layers_[current_layer_]->OnEvent(Layer::Event::ENTER);
    // display.TestProcedure();
}

void App::Loop()
{
    // Process current layer
    layers_[current_layer_]->Update();

    // Process animations
    if (!animation_runner.Update())
    {
        // No active animation, check for sleep
        if (layers_[current_layer_]->SleepAllowed())
        {
            Sleep();
        }
    }
}

void App::DisplayTime()
{
    auto now = rtc.GetDateTime();
    if (!now)
    {
        return; // Failed to get time
    }

    // Use LocaleConfig to process the time and determine animation parameters
    auto time_params = locale.ProcessTime(now->hour, now->minute);

    // Set the time animation with the processed parameters
    animation_runner.SetAnimation(Animation::Type::TIME, time_params);
}

void App::StartIntroAnimation()
{
    display.TriggerAutoBrightness();
    animation_runner.SetAnimation(Animation::Type::INTRO);
}

void App::StartChargeAnimation()
{
    display.TriggerAutoBrightness();
    animation_runner.SetAnimation(Animation::Type::CHARGE);
}

void App::Sleep()
{

    // Show battery level before sleep
    // float battery_level = battery.GetLevel();
    // display.SetNumber(battery_level * 12.f);
    // display.Update();
    // System::Delay(1000);

    display.DeInit();
    battery.DeInit();

    System::Sleep();

    display.Init();
    battery.Init();

    // Small delay for button debouncing after wakeup
    System::Delay(50);
}