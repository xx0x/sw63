#pragma once

#include "app/AnimationRunner.hpp"
#include "app/LocaleConfig.hpp"
#include "app/TimingsConfig.hpp"
#include "dev/Battery.hpp"
#include "dev/DS3231.hpp"
#include "dev/Display.hpp"
#include "dev/System.hpp"
#include <cstdint>

class App
{
public:
    static void Init();
    static void Loop();

    static void DisplayTime();
    static void StartIntroAnimation();
    static void StartChargeAnimation();

    inline static Display display;
    inline static TimingsConfig timings;
    inline static LocaleConfig locale;
    inline static AnimationRunner animation_runner;
    inline static DS3231 rtc = DS3231(&System::hi2c1);
    inline static Battery battery;

private:
    static void Sleep();

    // Animation state
    inline static uint32_t animation_delay_remaining_ = 0;
    inline static uint32_t last_animation_update_ = 0;
};
