
#include "LayerNormal.hpp"
#include "app/App.hpp"
#include "dev/System.hpp"

void LayerNormal::OnEvent(Event event)
{
    switch (event)
    {
    case Event::SHORT_PRESS:
        App::DisplayTime();
        break;
    case Event::MEDIUM_PRESS:
        App::ChangeLayer(Layer::Type::SETTINGS);
        break;
    case Event::CHARGE_START:
        App::StartChargeAnimation();
        break;
    case Event::ENTER:
        App::DisplayTime();
        break;
    case Event::LEAVE:
        // Handle layer leave event
        break;
    }
}

void LayerNormal::Update()
{
    bool current_button_state = System::GetRawButtonState();
    if (current_button_state && !prev_button_state_)
    {
        // Button was just pressed
        App::DisplayTime();
    }
    prev_button_state_ = current_button_state;
}