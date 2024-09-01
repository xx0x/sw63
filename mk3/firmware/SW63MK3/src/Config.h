#ifndef SW63_CONFIG_H
#define SW63_CONFIG_H

#include "Arduino.h"
#include "types.h"

namespace SW63
{
    class Config
    {
    public:
        static uint32_t TIME_HOLD;
        static uint32_t TIME_HOLD_DIGITS;
        static uint32_t TIME_PAUSE;
        static Language LANGUAGE;

        static void SetLanguage(Language language)
        {
            LANGUAGE = language;
        }

        static void SetSpeed(uint32_t speed)
        {
            if (speed < SPEED_COUNT)
            {
                TIME_HOLD = kHold[speed];
                TIME_HOLD_DIGITS = kHoldDigits[speed];
                TIME_PAUSE = kPause[speed];
            }
        }

    private:
        static constexpr uint32_t SPEED_COUNT = 6;
        static constexpr uint32_t kHoldDigits[SPEED_COUNT] = {1000, 800, 600, 400, 300, 150};
        static constexpr uint32_t kHold[SPEED_COUNT] = {800, 640, 480, 320, 240, 120};
        static constexpr uint32_t kPause[SPEED_COUNT] = {150, 120, 90, 60, 45, 23};
    };

}

#endif