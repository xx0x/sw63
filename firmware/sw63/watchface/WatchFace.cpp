#include "WatchFace.h";
#include <Arduino.h>;

WatchFace::WatchFace()
{
}

void WatchFace::updateLeds(bool minus, bool plus, bool hours, bool minutes, byte digit, byte face, bool pm)
{
    _minus = minus;
    _plus = plus;
    _hours = hours;
    _minutes = minutes;
    _digit = digit;
    _face = face;
    _pm = pm;
}

void WatchFace::doClock()
{
    digitalWrite(PIN_LED_CLOCK, HIGH);
    digitalWrite(PIN_LED_CLOCK, LOW);
}

void WatchFace::doClear()
{
    digitalWrite(PIN_LED_LATCH, LOW);
    digitalWrite(PIN_LED_DATA, LOW);
    for (byte i = 0; i < 24; i++)
        doClock();
    digitalWrite(PIN_LED_LATCH, HIGH);
}

void WatchFace::clearLeds()
{
    _minus = false;
    _plus = false;
    _hours = false;
    _minutes = false;
    _digit = 0;
    _face = 0;
    _pm = false;
}

void WatchFace::drawLeds()
{
    digitalWrite(PIN_LED_LATCH, LOW);
    doClock();
    doClock();
    doClock();
    digitalWrite(PIN_LED_DATA, _pm);
    doClock();
    for (byte i = 0; i < 4; i++)
    {
        digitalWrite(PIN_LED_DATA, (i == 4 - _face));
        doClock();
    }
    digitalWrite(PIN_LED_DATA, _minutes);
    doClock();
    digitalWrite(PIN_LED_DATA, _hours);
    doClock();
    digitalWrite(PIN_LED_DATA, _plus);
    doClock();
    digitalWrite(PIN_LED_DATA, _minus);
    doClock();

    for (byte i = 0; i < 12; i++)
    {
        digitalWrite(PIN_LED_DATA, (i >= 12 - _digit));
        doClock();
    }
    
    digitalWrite(PIN_LED_LATCH, HIGH);
}
