#ifndef SW63_ANIMATION_H
#define SW63_ANIMATION_H

#include "Arduino.h"
#include "../Hardware.h"
#include "../types.h"

namespace SW63
{
    class Animation
    {
    public:
        virtual void Init(Hardware *hw) = 0;
        virtual void Reset() = 0;
        virtual int Process() = 0;

        void SetTime(uint32_t hours, uint32_t minutes, ClockFace face, bool pm)
        {
            if (hours == 0)
            {
                hours = 12;
            }
            hours_ = hours;
            minutes_ = minutes;
            face_ = face;
            pm_ = pm;
        }

    protected:
        Hardware *hw_;
        uint32_t hours_ = 0;
        uint32_t minutes_ = 0;
        ClockFace face_ = CF_NONE;
        bool pm_ = false;
        uint32_t current_step_ = 0;
    };

}
#endif
