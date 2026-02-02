#include "LayerSettings.hpp"
#include "app/App.hpp"

void LayerSettings::OnEvent(Event event)
{
    switch (event)
    {
    case Event::ENTER:
        menu_step_ = 0;
        App::animation_runner.Cancel();
        break;
    case Event::SHORT_PRESS:
        // Cycle through menu steps
        menu_step_ = (menu_step_ + 1) % 12; // Example: 12 menu steps
        break;
    case Event::MEDIUM_PRESS:
        // Return to normal layer on long press
        App::ChangeLayer(Layer::Type::NORMAL);
        break;
    default:
        break;
    }
}

void LayerSettings::Update()
{
    App::display.Clear();
    App::display.SetPm(true); // Indicate settings mode with PM LED
    App::display.SetNumber(menu_step_ + 1, Display::NumStyle::BAR_REVERSED);
    App::display.Update();
}