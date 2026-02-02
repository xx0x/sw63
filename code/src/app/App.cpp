#include "App.hpp"
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

    DisplayTime();
    // display.TestProcedure();
}

void App::Loop()
{
    uint32_t current_time = HAL_GetTick();

    // Process animations if one is active
    if (!animation_runner.IsFinished())
    {
        // Check if it's time for the next animation frame
        if (current_time >= last_animation_update_ + animation_delay_remaining_)
        {
            // Handle pause between frames for time display animations
            if (animation_runner.ShouldPauseBetweenFrames())
            {
                display.Clear();
                display.Update();
                System::Delay(timings.GetSpeed().pause);

                // Update display brightness based on ambient light (the lights need to be off to read it properly)
                display.TriggerAutoBrightness();
            }

            // Process the next frame and get the delay until the next update
            animation_delay_remaining_ = animation_runner.ProcessNextFrame();
            last_animation_update_ = HAL_GetTick();

            // If the animation finished, clear the display
            if (animation_runner.IsFinished())
            {
                display.Clear();
                display.Update();
            }
        }
    }
    else
    {
        // Display time every 2 seconds
        // if (HAL_GetTick() - last_animation_update_ >= 2000)
        //{
        //    DisplayTime();
        //}

        if (System::GetRawChargeState())
        {
            // Start charge animation if charging
            StartChargeAnimation();
        }
        else
        {
            // No activity, go to sleep
            Sleep();

            if (System::GetRawButtonState())
            {
                // Wake up by button
                DisplayTime();
            }
            else if (System::GetRawChargeState())
            {
                // Wake up by charging
                StartChargeAnimation();
            }
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

    // Start the animation
    animation_delay_remaining_ = 0;
    last_animation_update_ = HAL_GetTick();
}

void App::StartIntroAnimation()
{
    display.TriggerAutoBrightness();
    animation_runner.SetAnimation(Animation::Type::INTRO);
    animation_delay_remaining_ = 0;
    last_animation_update_ = HAL_GetTick();
}

void App::StartChargeAnimation()
{
    display.TriggerAutoBrightness();
    animation_runner.SetAnimation(Animation::Type::CHARGE);
    animation_delay_remaining_ = 0;
    last_animation_update_ = HAL_GetTick();
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