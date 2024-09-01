#ifndef SW63_APPLICATION_H
#define SW63_APPLICATION_H

#include "Arduino.h"
#include "Hardware.h"
#include "AnimationProcessor.h"
#include "types.h"

namespace SW63
{
    class Application
    {
    public:
        void Init(voidFuncPtr button_callback);
        void Process();
        void ButtonCallback();
        void DisplayTime();
        void SmartDelay(uint32_t ms);

    private:
        Hardware hw_;
        bool button_pressed_;
        AnimationProcessor ap_;
    };

}

#endif