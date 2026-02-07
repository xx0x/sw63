#include "LayerSettings.hpp"
#include "app/App.hpp"

void LayerSettings::OnEvent(Event event)
{
    switch (event)
    {

    case Event::ENTER:
        settings_step_ = Step::LANGUAGE;
        App::animation_runner.Cancel();
        time_setting_ = TimeSetting(); // Clear settings
        break;

    case Event::JUST_RELEASED:
        ShortPressAction();
        break;

    case Event::MEDIUM_PRESS:
        if (settings_step_ == Step::PM)
        {
            // Finish settings and return to normal layer
            StoreSettings();
            App::ChangeLayer(Layer::Type::NORMAL);
        }
        else if (settings_step_ == Step::PAST_TO && time_setting_.past_to == PastTo::EXACT)
        {
            // Skip minutes step if EXACT is selected
            settings_step_ = Step::PM;
        }
        else
        {
            settings_step_ = EnumIncrement(settings_step_);
        }
        break;

    default:
        break;
    }
}

void LayerSettings::ShortPressAction()
{
    switch (settings_step_)
    {
    case Step::LANGUAGE:
        App::locale.SetLanguage(static_cast<LocaleConfig::Language>((std::to_underlying(App::locale.GetLanguage()) + 1) % std::to_underlying(LocaleConfig::Language::COUNT)));
        break;
    case Step::FACE:
        time_setting_.face = EnumIncrement(time_setting_.face);
        break;
    case Step::HOURS:
        time_setting_.hours = (time_setting_.hours + 1);
        if (time_setting_.hours > 12)
        {
            time_setting_.hours = 1;
        }
        break;
    case Step::PAST_TO:
        switch (time_setting_.past_to)
        {
        case PastTo::PAST:
            time_setting_.past_to = PastTo::TO;
            break;
        case PastTo::TO:
            time_setting_.past_to = PastTo::EXACT;
            break;
        case PastTo::EXACT:
            time_setting_.past_to = PastTo::PAST;
            break;
        }
        break;
    case Step::MINUTES:
        time_setting_.minutes = (time_setting_.minutes + 1);
        if (time_setting_.minutes > 12)
        {
            time_setting_.minutes = 1;
        }
        break;
    case Step::PM:
        time_setting_.pm = !time_setting_.pm;
        break;
    default:
        break;
    }
}

void LayerSettings::StoreSettings()
{
    int face_offset = std::to_underlying(time_setting_.face);
    int mn = face_offset * 15 +
             time_setting_.minutes * std::to_underlying(time_setting_.past_to);
    int hr = time_setting_.hours;

    if (App::locale.ShouldDecrementHour(time_setting_.face, mn))
    {
        hr--;
    }
    if (mn < 0)
    {
        mn += 60;
    }
    if (time_setting_.pm)
    {
        hr += 12;
    }
    if (hr == 24)
    {
        hr = 0;
    }

    App::rtc.SetDateTime({static_cast<uint8_t>(hr), static_cast<uint8_t>(mn), 0, 18, 8, 2025});
}

void LayerSettings::Update()
{
    App::display.Clear();

    switch (settings_step_)
    {
    case Step::LANGUAGE:
        App::display.SetPm(true);
        App::display.SetNumber(static_cast<size_t>(App::locale.GetLanguage()) + 1, Display::NumStyle::BAR_REVERSED);
        break;
    case Step::FACE:
        App::display.SetLed(Display::Led::HOURS, true);
        App::display.SetClockFace(time_setting_.face);
        break;
    case Step::HOURS:
        App::display.SetLed(Display::Led::HOURS, true);
        App::display.SetNumber(time_setting_.hours, Display::NumStyle::BAR_REVERSED);
        break;
    case Step::PAST_TO:
        App::display.SetLed(Display::Led::MINUTES, true);
        App::display.SetLed(Display::Led::BEFORE, time_setting_.past_to == PastTo::TO);
        App::display.SetLed(Display::Led::AFTER, time_setting_.past_to == PastTo::PAST);
        break;
    case Step::MINUTES:
        App::display.SetLed(Display::Led::MINUTES, true);
        App::display.SetLed(Display::Led::BEFORE, time_setting_.past_to == PastTo::TO);
        App::display.SetLed(Display::Led::AFTER, time_setting_.past_to == PastTo::PAST);
        App::display.SetNumber(time_setting_.minutes, Display::NumStyle::BAR_REVERSED);
        break;
    case Step::PM:
        App::display.SetLed(Display::Led::PM, time_setting_.pm);
        break;
    }

    App::display.Update();
}