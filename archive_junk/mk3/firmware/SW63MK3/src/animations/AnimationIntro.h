#ifndef SW63_ANIMATION_INTRO_H
#define SW63_ANIMATION_INTRO_H

#include <Arduino.h>
#include "Animation.h"
#include "../Config.h"

namespace SW63
{
    class AnimationIntro : public Animation
    {

    public:
        uint32_t Process()
        {
            length_ = 26;
            
            if (current_step_ < 12)
            {
                hw_->SetLeds(current_step_ % 2 == 0, current_step_ % 5 == 0, current_step_ % 3 == 0, current_step_ % 4 == 0, current_step_ + 1, CF_NONE, false);
            }
            else if (current_step_ < 16)
            {
                hw_->SetLeds(false, false, false, false, 0, (ClockFace)(current_step_ - 11), current_step_ % 2 == 0);
            }

            if (current_step_ < 16)
            {
                current_step_++;
                return 120;
            }

            if (current_step_ < 26)
            {
                if (current_step_ % 2 == 0)
                {

                    hw_->SetLeds(false, false, false, false, Config::LANGUAGE + 1, CF_NONE, false);
                }
                else
                {
                    hw_->ClearLeds();
                }
                current_step_++;
                return 100;
            }

            return 0;
        }
    };
}

#endif