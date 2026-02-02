
#include "LayerNormal.hpp"
#include "app/App.hpp"
#include "dev/System.hpp"

void LayerNormal::OnEvent(Event event)
{
    switch (event)
    {
    case Event::INIT:
        // Initialization code here
        break;
    case Event::SHORT_PRESS:
        // Handle button press event
        break;
    case Event::CHARGE_START:
        // Handle charge start event
        break;
    case Event::ENTER:
        // Handle layer enter event
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