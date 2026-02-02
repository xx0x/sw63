#pragma once

class Layer
{
public:
    enum class Type
    {
        NORMAL,
        SETTINGS,
        COUNT
    };
    enum class Event
    {
        SHORT_PRESS,  // pressed and released under 2 seconds
        MEDIUM_PRESS, // hold over 2 seconds
        LONG_PRESS,   // hold over 5 seconds
        DOUBLE_PRESS, // two quick presses under 0.5 seconds
        MULTI_PRESS,  // five quick presses under 2.5 seconds
        CHARGE_START,
        ENTER,
        LEAVE
    };
    virtual void OnEvent(Event event) = 0;
    virtual void Update() = 0;
    virtual bool SleepAllowed() = 0;
};