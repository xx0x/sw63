#include "LayerSecret.hpp"
#include "app/App.hpp"

void LayerSecret::OnEvent(Event event)
{
    switch (event)
    {

    case Event::ENTER:
        current_setting_ = Settings::SPEED;
        setting_opened_ = false;
        break;

    case Event::JUST_RELEASED:
        ShortPressAction();
        break;

    case Event::MEDIUM_PRESS:
        setting_opened_ = !setting_opened_;
        if (setting_opened_ && current_setting_ == Settings::EXIT)
        {
            App::ChangeLayer(Layer::Type::NORMAL);
        }
        break;

    default:
        break;
    }
}

void LayerSecret::ShortPressAction()
{
    if (!setting_opened_)
    {
        current_setting_ = EnumIncrement(current_setting_);
        return;
    }

    switch (current_setting_)
    {
    case Settings::SPEED:
        App::timings.SetSpeed((App::timings.GetSpeed() + 1) % App::timings.GetSpeedCount());
        break;
    case Settings::SAVER:
    case Settings::COMPRESSED:
    case Settings::EXIT:
    default:
        // Todo: implement these settings
        break;
    }
}

void LayerSecret::Update()
{
    App::display.Clear();

    switch (current_setting_)
    {
    case Settings::SPEED:
        App::display.SetLed(Display::Led::MINUTES, true);
        break;
    case Settings::COMPRESSED:
        App::display.SetLed(Display::Led::HOURS, true);
        break;
    case Settings::SAVER:
        App::display.SetLed(Display::Led::AFTER, true);
        break;
    case Settings::EXIT:
        App::display.SetLed(Display::Led::BEFORE, true);
        break;
    case Settings::COUNT:
    default:
        break;
    }

    if (!setting_opened_)
    {
        App::display.Update();
        return;
    }
    App::display.SetPm(true);

    switch (current_setting_)
    {
    case Settings::SPEED:
        App::display.SetNumber(App::timings.GetSpeed() + 1, Display::NumStyle::BAR_REVERSED);
        break;
    case Settings::SAVER:
    case Settings::COMPRESSED:
    case Settings::EXIT:
    default:
        // Todo: implement these settings
        break;
    }

    App::display.Update();
}