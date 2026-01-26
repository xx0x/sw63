#ifndef SW63_ANIMATION_PAST_H
#define SW63_ANIMATION_PAST_H

#include <Arduino.h>
#include "Animation.h"
#include "../Config.h"

namespace SW63
{

    class AnimationPast : public Animation
    {

    public:
        uint32_t Process()
        {
            uint32_t delay_for = 0;
            length_ = 7;

            if (Config::LANGUAGE == LANG_HUNGARIAN)
            {
                switch (current_step_)
                {
                case 0:
                    hw_->SetLeds(false, true, false, false, 0, CF_NONE, false); // a
                    delay_for = Config::TIME_HOLD;
                    break;
                case 1:
                    hw_->SetLeds(false, false, false, false, minutes_, CF_NONE, false); // osm
                    delay_for = Config::TIME_HOLD_DIGITS;
                    break;
                case 2:
                    hw_->SetLeds(false, false, false, true, 0, CF_NONE, false); // minut
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
            }
            else
            {
                switch (current_step_)
                {
                case 0:
                    hw_->SetLeds(false, false, false, false, 0, face_, false); // pul
                    delay_for = Config::TIME_HOLD;
                    break;
                case 1:
                    hw_->SetLeds(false, false, false, false, hours_, CF_NONE, false); // ctvrte
                    delay_for = Config::TIME_HOLD_DIGITS;
                    break;
                case 2:
                    hw_->SetLeds(false, false, true, false, 0, CF_NONE, false); // hodiny
                    delay_for = Config::TIME_HOLD;
                    break;
                case 3:
                    hw_->SetLeds(false, true, false, false, 0, CF_NONE, false); // a
                    delay_for = Config::TIME_HOLD;
                    break;
                case 4:
                    hw_->SetLeds(false, false, false, false, minutes_, CF_NONE, false); // osm
                    delay_for = Config::TIME_HOLD_DIGITS;
                    break;
                case 5:
                    hw_->SetLeds(false, false, false, true, 0, CF_NONE, false); // minut
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
            }
            current_step_++;
            return delay_for;
        }
    };

}

#endif