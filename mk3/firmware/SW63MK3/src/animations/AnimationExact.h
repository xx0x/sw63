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
            int delayFor = 0;

            switch (current_step_)
            {
            case 0:
                hw_->SetLeds(false, false, false, false, 0, face_, false); // pul
                delayFor = Config::TIME_HOLD;
                break;
            case 1:
                hw_->SetLeds(false, false, false, false, hours_, CF_NONE, false); // ctvrte
                delayFor = Config::TIME_HOLD_DIGITS;
                break;
            case 2:
                hw_->SetLeds(false, false, true, false, 0, CF_NONE, false); // hodiny
                delayFor = Config::TIME_HOLD;
                break;
            case 3:
                if (pm_)
                {
                    hw_->SetLeds(false, false, false, false, 0, CF_NONE, true); // PM
                    delayFor = Config::TIME_HOLD;
                }
                break;
            }
            current_step_++;
            return delayFor;
        }
    };

}

#endif