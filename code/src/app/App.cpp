#include "App.hpp"
#include "app/ui/LayerNormal.hpp"
#include "app/ui/LayerSecret.hpp"
#include "app/ui/LayerSettings.hpp"
#include "dev/System.hpp"

void App::Init()
{
    display.Init();
    battery.Init();

    // Initialize RTC with retries in case of some issues at startup
    bool rtc_success = false;
    for (int i = 0; i < 10; i++)
    {
        rtc_success = rtc.Init();
        if (rtc_success)
        {
            break;
        }
        System::Delay(200);
    }
    if (!rtc_success)
    {
        while (1)
        {
            display.Clear();
            display.SetPm(true); // Indicate error with PM LED
            display.Update();
            System::Delay(500);
            display.Clear();
            display.SetPm(false);
            display.Update();
            System::Delay(500);
        }
    }

    // Set test date (we don't care about the date here, just time)
    rtc.SetDateTime({0, 0, 0, 18, 8, 2025});

    layers_[Layer::Type::NORMAL] = std::make_unique<LayerNormal>();
    layers_[Layer::Type::SETTINGS] = std::make_unique<LayerSettings>();
    layers_[Layer::Type::SECRET] = std::make_unique<LayerSecret>();

    // Init all layers
    for (auto &layer : layers_)
    {
        layer->OnEvent(Layer::Event::INIT);
    }

    // We don't call Layer::Event::ENTER here so the DisplayTime inside LayerNormal isn't triggered
    // ...
}

void App::Loop()
{
    // Update button state
    button.Update();

    // Call button events here
    if (button.Happened(Button::Event::JUST_PRESSED))
    {
        layers_[current_layer_]->OnEvent(Layer::Event::JUST_PRESSED);
        UpdateLastInteractionTime();
    }
    if (button.Happened(Button::Event::JUST_RELEASED))
    {
        layers_[current_layer_]->OnEvent(Layer::Event::JUST_RELEASED);
        UpdateLastInteractionTime();
    }
    if (button.Happened(Button::Event::LONG_PRESS))
    {
        layers_[current_layer_]->OnEvent(Layer::Event::LONG_PRESS);
        UpdateLastInteractionTime();
    }
    if (button.Happened(Button::Event::MEDIUM_PRESS))
    {
        layers_[current_layer_]->OnEvent(Layer::Event::MEDIUM_PRESS);
        UpdateLastInteractionTime();
    }
    if (button.Happened(Button::Event::MULTI_PRESS))
    {
        layers_[current_layer_]->OnEvent(Layer::Event::MULTI_PRESS);
        UpdateLastInteractionTime();
    }

    // Process current layer
    layers_[current_layer_]->Update();

    // Check for timeout
    if (current_layer_ != Layer::Type::NORMAL &&
        (System::Millis() - last_interaction_time_ >= Settings::kInactivityTimeoutMs))
    {
        ChangeLayer(Layer::Type::NORMAL);
    }

    // Check for sleep (only if no USB power and button not pressed)
    if (
        layers_[current_layer_]->SleepAllowed() &&
        !System::GetRawUsbPowerState() &&
        !System::GetRawButtonState())
    {
        Sleep();
    }
}

void App::Sleep()
{
    // Indicate low battery
    float battery_level = battery.GetLevel();
    if (battery_level < Settings::kLowBatteryThreshold)
    {
        for (int i = 0; i < 3; i++)
        {
            display.Clear();
            display.Update();
            System::Delay(200);
            display.Clear();
            display.SetLed(Display::Led::PM, true);
            display.SetLed(Display::Led::NUM_1, true);
            display.Update();
            System::Delay(200);
        }
    }

    display.DeInit();
    battery.DeInit();

    System::Sleep();

    display.Init();
    battery.Init();
}