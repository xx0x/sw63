#ifndef SW63_ANIMATION_CHARGE_H
#define SW63_ANIMATION_CHARGE_H

#include <Arduino.h>
#include "Animation.h"
#include "../Config.h"

namespace SW63
{
    class AnimationCharge : public Animation
    {

    public:
        uint32_t Process()
        {
            length_ = UINT32_MAX;

            // Keep full bar
            bool finished_charging = !hw_->IsCharging() && hw_->IsChargerConnected();
            if (finished_charging)
            {
                hw_->SetLeds(0, 0, 0, 0, 12, CF_NONE, false);
                return 500;
            }

            // Stop animation
            if (!hw_->IsCharging())
            {
                current_step_ = UINT32_MAX;
                return 0;
            }

            // Animate charging
            hw_->SetLeds(0, 0, 0, 0, current_step_, CF_NONE, false);
            current_step_++;
            if (current_step_ > 12)
            {
                current_step_ = 0;
            }
            return 500;
        }
    };
}

#endif