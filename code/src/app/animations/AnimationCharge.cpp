#include "AnimationCharge.hpp"
#include "app/App.hpp"

uint32_t AnimationCharge::ProcessNextFrame()
{
    if (current_step_ >= Display::kNumLedCount)
    {
        // Restart animation for continuous charging indicator
        current_step_ = 0;
    }

    bool usb_power = System::GetRawUsbPowerState();
    bool charging = System::GetRawChargeState();

    App::display.Clear();

    if (charging)
    {
        const uint32_t led_number = std::clamp(static_cast<size_t>(current_step_ + 1u),
                                               static_cast<size_t>(1u),
                                               static_cast<size_t>(Display::kNumLedCount));

        App::display.SetNumber(led_number,
                               Display::NumStyle::SNAKE_REVERSED);
    }
    else
    {
        // Blink the full bar if charged and on USB power
        if (current_step_ % 2 == 0)
        {
            App::display.SetNumber(12, Display::NumStyle::BAR);
        }
    }

    App::display.Update();
    current_step_++;

    // Continue animation only while charging or USB power is connected
    if (!usb_power)
    {
        finished_ = true;
        return 0;
    }

    // Not charging, make the flashing slow
    if (!charging)
    {
        return 1000;
    }

    // Charging, normal animation speed
    return 150;
}