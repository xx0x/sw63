#include "Application.h"
#include "Config.h"

namespace SW63
{

    void Application::Init(voidFuncPtr button_callback)
    {
        button_pressed_ = false;

        Config::SetSpeed(0);
        Config::SetLanguage(LANG_CZECH);

        hw_.Init(button_callback);
        ap_.Init(&hw_);

        // Set intro animation
        ap_.SetAnimation(AT_INTRO);

        // Check brightness on startup
        hw_.AutoBrightness();
    }

    void Application::Process()
    {
        if (button_pressed_)
        {
            DisplayTime();
            button_pressed_ = false;
        }

        uint32_t wait = ap_.Process();
        if (wait > 0)
        {
            hw_.TransferLeds();
            SmartDelay(wait);
            if (ap_.PauseBetweenFrames())
            {
                // hw_.ClearLeds();
                // hw_.TransferLeds();
                // Auto brightness actually clears the leds for us
                hw_.AutoBrightness();
                delay(Config::TIME_PAUSE);
            }
        }
        else
        {
            ap_.Reset();
            hw_.Sleep();

            // Check brightness on wakeup
            hw_.AutoBrightness();
        }
    }

    void Application::ButtonCallback()
    {
        button_pressed_ = true;
    }

    void Application::SmartDelay(uint32_t ms)
    {
        for (uint32_t i = 0; i < ms; i += 10)
        {
            if (button_pressed_)
            {
                break;
            }
            delay(10);
        }
    }

    void Application::DisplayTime()
    {
        // Clear LEDs
        hw_.ClearLeds();

        int hh = 16;
        int mm = 20;

        uint32_t aftercoef = 0; // how "quarter after" modifies hours
        uint32_t halfcoef = 0;  // how "half past" modifies hours
        uint32_t tocoef = 0;    // "how quarter to" modifies hours

        if (Config::LANGUAGE == LANG_CZECH || Config::LANGUAGE == LANG_HUNGARIAN)
        {
            aftercoef = 1;
            halfcoef = 1;
            tocoef = 1;
        }
        else if (Config::LANGUAGE == LANG_ENGLISH)
        {
            aftercoef = 0;
            halfcoef = 0;
            tocoef = 1;
        }
        else if (Config::LANGUAGE == LANG_GERMAN || Config::LANGUAGE == LANG_POLISH)
        {
            aftercoef = 0;
            halfcoef = 1;
            tocoef = 1;
        }

        uint32_t minutes = mm;
        uint32_t hours = hh % 12;
        bool pm = (hh >= 12);

        if (minutes == 0)
        {
            ap_.SetAnimation(AT_EXACT, hours, 0, CF_UP, pm);
        }
        else if (minutes < 11)
        {
            ap_.SetAnimation(AT_PAST, hours, minutes, CF_UP, pm);
        }
        else if (minutes < 15)
        {
            ap_.SetAnimation(AT_TO, hours + aftercoef, 15 - minutes, CF_RIGHT, pm);
        }
        else if (minutes == 15)
        {
            ap_.SetAnimation(AT_EXACT, hours + aftercoef, 0, CF_RIGHT, pm);
        }
        else if (minutes < 20)
        {
            ap_.SetAnimation(AT_PAST, hours + aftercoef, minutes - 15, CF_RIGHT, pm);
        }
        else if (minutes < 30)
        {
            ap_.SetAnimation(AT_TO, hours + halfcoef, 30 - minutes, CF_DOWN, pm);
        }
        else if (minutes == 30)
        {
            ap_.SetAnimation(AT_EXACT, hours + halfcoef, 0, CF_DOWN, pm);
        }
        else if (minutes < 41)
        {
            ap_.SetAnimation(AT_PAST, hours + halfcoef, minutes - 30, CF_DOWN, pm);
        }
        else if (minutes < 45)
        {
            ap_.SetAnimation(AT_TO, hours + tocoef, 45 - minutes, CF_LEFT, pm);
        }
        else if (minutes == 45)
        {
            ap_.SetAnimation(AT_EXACT, hours + tocoef, 0, CF_LEFT, pm);
        }
        else if (minutes < 50)
        {
            ap_.SetAnimation(AT_PAST, hours + tocoef, minutes - 45, CF_LEFT, pm);
        }
        else
        {
            ap_.SetAnimation(AT_TO, hours + 1, 60 - minutes, CF_UP, pm);
        }
    }
}