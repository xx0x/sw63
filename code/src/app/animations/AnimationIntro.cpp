#include "AnimationIntro.hpp"
#include "app/App.hpp"

uint32_t AnimationIntro::ProcessNextFrame()
{
    static constexpr uint32_t kTotalSteps = 20;

    if (current_step_ >= kTotalSteps)
    {
        finished_ = true;
        return 0;
    }

    App::display.Clear();

    uint32_t delay_ms = 100; // Fast intro animation

    if (current_step_ < 12)
    {
        // Animate through numbers and LEDs
        if (current_step_ % 2 == 0)
            App::display.SetTopLed(Display::TopLed::AFTER);
        if (current_step_ % 5 == 0)
            App::display.SetTopLed(Display::TopLed::BEFORE);
        if (current_step_ % 3 == 0)
            App::display.SetTopLed(Display::TopLed::HOURS);
        if (current_step_ % 4 == 0)
            App::display.SetTopLed(Display::TopLed::MINUTES);

        App::display.SetNumber(current_step_ + 1);
    }
    else if (current_step_ < 16)
    {
        // Cycle through clock faces
        auto face = static_cast<Display::ClockFace>(current_step_ - 12);
        App::display.SetClockFace(face);
        App::display.SetPm(current_step_ % 2 == 0);
    }
    else
    {
        // Show language indicator
        uint32_t lang_num = static_cast<uint32_t>(App::locale.GetLanguage()) + 1;
        App::display.SetNumber(current_step_ % 2 == 0 ? lang_num : 0);
    }

    App::display.Update();
    current_step_++;

    return delay_ms;
}