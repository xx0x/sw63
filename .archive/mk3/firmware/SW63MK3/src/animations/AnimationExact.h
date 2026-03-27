#ifndef SW63_ANIMATION_EXACT_H
#define SW63_ANIMATION_EXACT_H

#include "Arduino.h"
#include "Animation.h"
#include "../Config.h"

namespace SW63
{

    class AnimationExact : public Animation
    {

    public:
        uint32_t Process()
        {
            uint32_t delay_for = 0;
            length_ = 4;

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