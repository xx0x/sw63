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
        JUST_PRESSED,
        MEDIUM_PRESS,
        LONG_PRESS,
        MULTI_PRESS,
        ENTER,
        LEAVE
    };
    virtual void OnEvent(Event event) = 0;
    virtual void Update() = 0;
    virtual bool SleepAllowed() = 0;
};