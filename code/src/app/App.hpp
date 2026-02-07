#pragma once

#include "app/LocaleConfig.hpp"
#include "app/TimingsConfig.hpp"
#include "dev/Battery.hpp"
#include "dev/Button.hpp"
#include "dev/DS3231.hpp"
#include "dev/Display.hpp"
#include "dev/System.hpp"
#include "ui/Layer.hpp"
#include <cstdint>
#include <memory>

class App
{
public:
    static void Init();
    static void Loop();

    inline static Display display;
    inline static TimingsConfig timings;
    inline static LocaleConfig locale;
    inline static DS3231 rtc = DS3231(&System::hi2c1);
    inline static Battery battery;
    inline static Button button;

    static void ChangeLayer(Layer::Type layer_type)
    {
        layers_[current_layer_]->OnEvent(Layer::Event::LEAVE);
        current_layer_ = layer_type;

        // Short delay before entering new layer
        // Just a visual thing
        App::display.Clear();
        App::display.Update();
        System::Delay(20);

        layers_[current_layer_]->OnEvent(Layer::Event::ENTER);
    }

    static constexpr uint32_t kInactivityTimeoutMs = 15000;

private:
    static void Sleep();

    static inline EnumArray<Layer::Type, std::unique_ptr<Layer>> layers_;
    static inline Layer::Type current_layer_ = Layer::Type::NORMAL;

    static void UpdateLastInteractionTime()
    {
        last_interaction_time_ = System::Millis();
    }
    static inline uint32_t last_interaction_time_ = 0;
};
