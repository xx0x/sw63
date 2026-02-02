#include "LayerSettings.hpp"
#include "app/App.hpp"

void LayerSettings::Update()
{
    App::display.Clear();
    App::display.SetPm(true); // Indicate settings mode with PM LED
    App::display.Update();
}