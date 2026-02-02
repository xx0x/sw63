
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
    case Event::MULTI_PRESS:
        App::StartIntroAnimation();
        break;
    case Event::CHARGE_START:
        App::StartChargeAnimation();
        break;
    case Event::ENTER:
        App::DisplayTime();
        break;
    case Event::LEAVE:
        App::animation_runner.Cancel();
        break;
    }
}

void LayerNormal::Update()
{
}