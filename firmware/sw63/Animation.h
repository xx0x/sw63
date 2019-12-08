#ifndef Animation_h
#define Animation_h

#include "Arduino.h"

class Animation
{
public:
   
    virtual void setup();
    virtual int draw();

    void setTime(byte hours, byte minutes, byte face, bool pm)
    {
        if(hours == 0) hours = 12;
        _hours = hours;
        _minutes = minutes;
        _face = face;
        _pm = pm;
    }

protected:
    byte _hours;
    byte _minutes;
    byte _face;
    bool _pm;
};

#endif
