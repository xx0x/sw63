#ifndef WATCHFACE_H
#define WATCHFACE_H

#include <Arduino.h>

class WatchFace
{

public:
    WatchFace();

    void updateLeds(bool minus, bool plus, bool hours, bool minutes, byte digit, byte face, bool pm);
    void drawLeds();
    void clearLeds();

protected:
    void doClear();
    void doClock();
    bool _minus;
    bool _plus;
    bool _hours;
    bool _minutes;
    byte _digit;
    byte _face;
    bool _pm;
};

#endif
