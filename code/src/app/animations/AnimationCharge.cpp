#include "AnimationCharge.hpp"
#include "app/App.hpp"

uint32_t AnimationCharge::ProcessNextFrame()
{
    if (current_step_ >= Display::kNumLedCount)
    {
        // Restart animation for continuous charging indicator
        current_step_ = 0;
    }

    const uint32_t led_number = std::clamp(static_cast<size_t>(current_step_ + 1u),
                                           static_cast<size_t>(1u),
                                           static_cast<size_t>(Display::kNumLedCount));

    App::display.Clear();
    App::display.SetNumber(led_number,
                           Display::NumStyle::SNAKE_REVERSED);

    App::display.Update();
    current_step_++;

    // Continue animation only while charging
    if (!System::GetRawChargeState())
    {
        finished_ = true;
        return 0;
    }
    return 150; // Moderate speed for charge animation
}