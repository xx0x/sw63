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
        ENTER,
        LEAVE,
        JUST_PRESSED,
        JUST_RELEASED,
        MEDIUM_PRESS,
        LONG_PRESS,
        MULTI_PRESS,
        COUNT
    };

    virtual void OnEvent(Event event) = 0;
    virtual void Update() = 0;
    virtual bool SleepAllowed() = 0;
};