#include "App.hpp"
#include "app/ui/LayerNormal.hpp"
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
    rtc.SetDateTime({16, 20, 0, 18, 8, 2025});

    // Set default speed
    timings.SetSpeed(0);

    layers_[Layer::Type::NORMAL] = std::make_unique<LayerNormal>();
    layers_[Layer::Type::SETTINGS] = std::make_unique<LayerSettings>();

    layers_[current_layer_]->OnEvent(Layer::Event::ENTER);
    // display.TestProcedure();
}

void App::Loop()
{
    // Update button state
    button.Update();

    // Call button events here
    if (button.Happened(Button::Event::SHORT_PRESS))
    {
        layers_[current_layer_]->OnEvent(Layer::Event::SHORT_PRESS);
    }
    if (button.Happened(Button::Event::LONG_PRESS))
    {
        layers_[current_layer_]->OnEvent(Layer::Event::LONG_PRESS);
    }
    if (button.Happened(Button::Event::MEDIUM_PRESS))
    {
        layers_[current_layer_]->OnEvent(Layer::Event::MEDIUM_PRESS);
    }
    if (button.Happened(Button::Event::MULTI_PRESS))
    {
        layers_[current_layer_]->OnEvent(Layer::Event::MULTI_PRESS);
    }

    // Process current layer
    layers_[current_layer_]->Update();

    // Process animations
    if (!animation_runner.Update())
    {
        if (System::GetRawUsbPowerState())
        {
            App::display.TriggerAutoBrightness();
            App::animation_runner.SetAnimation(Animation::Type::CHARGE);
        }
        else
        {
            // No active animation, check for sleep
            if (layers_[current_layer_]->SleepAllowed() &&
                !System::GetRawButtonState())
            {
                Sleep();
            }
        }
    }
}

void App::Sleep()
{

    // Show battery level before sleep
    // float battery_level = battery.GetLevel();
    // display.SetNumber(battery_level * 12.f);
    // display.Update();
    // System::Delay(1000);

    display.DeInit();
    battery.DeInit();

    System::Sleep();

    display.Init();
    battery.Init();

    // current_layer_ = Layer::Type::NORMAL;
    // layers_[current_layer_]->OnEvent(Layer::Event::ENTER);
}