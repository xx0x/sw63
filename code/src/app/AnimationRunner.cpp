#include "AnimationRunner.hpp"
#include "App.hpp"

bool AnimationRunner::Update()
{
    // If no animation is running, nothing to do
    if (IsFinished())
    {
        current_type_ = Animation::Type::COUNT;
        return false;
    }

    uint32_t current_time = HAL_GetTick();

    // Check if it's time for the next animation frame
    if (current_time >= last_animation_update_ + animation_delay_remaining_)
    {
        // Handle pause between frames for time display animations
        if (current_type_ == Animation::Type::TIME)
        {
            App::display.Clear();
            App::display.Update();
            System::Delay(App::timings.GetSpeed().pause);

            // Update display brightness based on ambient light (the lights need to be off to read it properly)
            App::display.TriggerAutoBrightness();
        }

        // Process the next frame and get the delay until the next update
        animation_delay_remaining_ = ProcessNextFrame();
        last_animation_update_ = HAL_GetTick();

        // If the animation finished, clear the display
        if (IsFinished())
        {
            App::display.Clear();
            App::display.Update();
            current_type_ = Animation::Type::COUNT;
            return false; // Animation is done
        }
    }

    return true; // Animation is still running
}