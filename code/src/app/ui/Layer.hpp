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
        SHORT_PRESS,
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