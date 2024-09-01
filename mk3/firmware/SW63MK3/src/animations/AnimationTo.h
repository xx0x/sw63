#ifndef SW63_ANIMATION_TO_H
#define SW63_ANIMATION_TO_H

#include <Arduino.h>
#include "Animation.h"
#include "../Config.h"

namespace SW63
{

    class AnimationTo : public Animation
    {

    public:
        void Init(Hardware *hw)
        {
            hw_ = hw;
            Reset();
        }

        void Reset()
        {
            current_step_ = 0;
        }

        int Process()
        {
            int delay_for = 0;
            switch (current_step_)
            {
            case 0:
                if (Config::LANGUAGE == LANG_CZECH || Config::LANGUAGE == LANG_POLISH)
                {
                    hw_->SetLeds(true, false, false, false, 0, CF_NONE, false); // za
                    delay_for = Config::TIME_HOLD;
                }
                else
                {
                    hw_->SetLeds(false, false, false, false, minutes_, CF_NONE, false); // eight
                    delay_for = Config::TIME_HOLD_DIGITS;
                }
                break;
            case 1:
                if (Config::LANGUAGE == LANG_CZECH || Config::LANGUAGE == LANG_POLISH)
                {
                    hw_->SetLeds(false, false, false, false, minutes_, CF_NONE, false); // osm
                    delay_for = Config::TIME_HOLD_DIGITS;
                }
                else
                {
                    hw_->SetLeds(false, false, false, true, 0, CF_NONE, false); // minutes
                    delay_for = Config::TIME_HOLD;
                }
                break;
            case 2:
                if (Config::LANGUAGE == LANG_CZECH || Config::LANGUAGE == LANG_POLISH)
                {
                    hw_->SetLeds(false, false, false, true, 0, CF_NONE, false); // minut
                }
                else
                {
                    hw_->SetLeds(true, false, false, false, 0, CF_NONE, false); // to
                }
                delay_for = Config::TIME_HOLD;
                break;
            case 3:
                hw_->SetLeds(false, false, false, false, 0, face_, false); // pul
                delay_for = Config::TIME_HOLD;
                break;
            case 4:
                hw_->SetLeds(false, false, false, false, hours_, CF_NONE, false); // ctvrte
                delay_for = Config::TIME_HOLD_DIGITS;
                break;
            case 5:
                hw_->SetLeds(false, false, true, false, 0, CF_NONE, false); // hodiny
                delay_for = Config::TIME_HOLD;
                break;
            case 6:
                if (pm_)
                {
                    hw_->SetLeds(false, false, false, false, 0, CF_NONE, true); // PM
                    delay_for = Config::TIME_HOLD;
                }
                break;
            }
            current_step_++;
            return delay_for;
        }
    };

}

#endif