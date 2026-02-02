#pragma once

#include "app/AnimationRunner.hpp"
#include "app/LocaleConfig.hpp"
#include "app/TimingsConfig.hpp"
#include "dev/Battery.hpp"
#include "dev/Button.hpp"
#include "dev/DS3231.hpp"
#include "dev/Display.hpp"
#include "dev/System.hpp"
#include "ui/Layer.hpp"
#include <cstdint>

class App
{
public:
    static void Init();
    static void Loop();

    inline static Display display;
    inline static TimingsConfig timings;
    inline static LocaleConfig locale;
    inline static AnimationRunner animation_runner;
    inline static DS3231 rtc = DS3231(&System::hi2c1);
    inline static Battery battery;
    inline static Button button;

    static void ChangeLayer(Layer::Type layer_type)
    {
        layers_[current_layer_]->OnEvent(Layer::Event::LEAVE);
        current_layer_ = layer_type;
        layers_[current_layer_]->OnEvent(Layer::Event::ENTER);
    }

private:
    static void Sleep();

    static inline EnumArray<Layer::Type, std::unique_ptr<Layer>> layers_;
    static inline Layer::Type current_layer_ = Layer::Type::NORMAL;
};
