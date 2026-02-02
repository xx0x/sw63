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
        SHORT_PRESS,  // under 2 seconds
        MEDIUM_PRESS, // over 2 seconds
        LONG_PRESS,   // over 5 seconds
        DOUBLE_PRESS, // two quick presses under 0.5 seconds
        CHARGE_START,
        ENTER,
        LEAVE
    };
    virtual void OnEvent(Event event) = 0;
    virtual void Update() = 0;
    virtual bool SleepAllowed() = 0;
};