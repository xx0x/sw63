// SW63 SPACEWATCH SOURCE CODE
// for MCU: ATtiny 84 (ATTinyCore) 8 MHz
// author: Vaclav Mach (vaclavmach@hotmail.cz, info@xx0x.cz)
// project homepage: www.xx0x.cz

#include <avr/interrupt.h>
#include <avr/sleep.h>

#include <DS3232RTC.h> // https://github.com/JChristensen/DS3232RTC
#include <TinyWireM.h>
#include <TimeLib.h>

// pin definitions - see schematic
#define PIN_ENABLE 3
#define PIN_LED_DATA 2
#define PIN_LED_CLOCK 1
#define PIN_LED_LATCH 0
#define PIN_LED_PWM 8
#define PIN_LIGHT_SENSOR A7
#define PIN_BUTTON 5

// how long in ms LEDs should stay on
#define HOLD_DIGITS 1000
#define HOLD 800
#define PAUSE 150
#define SKIP 1

// language defintions
#define LANGUAGES_COUNT 6
#define CZECH 1
#define ENGLISH 2
#define GERMAN 3
#define FRENCH 4
#define POLISH 5
#define HUNGARIAN 6

// default language
byte language = CZECH;

// custom display class
#include "watchface/WatchFace.h"
#include "watchface/WatchFace.cpp"
WatchFace watchFace = WatchFace();

// animation definitions
#include "Animation.h"
#include "animations/AnimationExact.cpp"
#include "animations/AnimationPast.cpp"
#include "animations/AnimationTo.cpp"
#include "animations/AnimationIntro.cpp"
#include "animations/AnimationFrench.cpp"
Animation *animation;

// button and timings
bool buttonPressed = false;
unsigned long lastTimeButton = 0;
unsigned long smashingStarted = 0;
byte smashingCount = 0;
#define SMASH_INTERVAL 2000
#define SMASH_COUNT 3
#define LONG_PRESS 1500
#define XLONG_PRESS 4000
#define DEBOUNCE_PRESS 10

// animation timings
unsigned long nextAnimationIsAt = 0;
unsigned long nextPauseIsAt = 0;
bool reloadTime = true;
bool paused = true;

// settings definitions
#define SETTING_STEPS 6
#define SETTING_LANGUAGE 0
#define SETTING_FACE 1
#define SETTING_HOURS 2
#define SETTING_PASTTO 3
#define SETTING_MINUTES 4
#define SETTING_PM 5
bool settingTime = false;
byte currentSetting = 0;
bool ignoreFirstSettingClick = false;

// variables for storing when setting time
byte _face = 1;
byte _hours = 1;
byte _minutes = 1;
int _pastTo = 1;
bool _pm = true;

// how often should brightness check occur
#define BRIGHTNESS_CHECK_INTERVAL 50
unsigned long lastTimeBrightnessCheck = 0;


void setup()
{
    pinMode(PIN_ENABLE, OUTPUT);
    pinMode(PIN_LED_DATA, OUTPUT);
    pinMode(PIN_LED_LATCH, OUTPUT);
    pinMode(PIN_LED_CLOCK, OUTPUT);
    pinMode(PIN_LED_PWM, OUTPUT);
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_LIGHT_SENSOR, INPUT);

    digitalWrite(PIN_ENABLE, true);

    delay(10);
    setClock(0, 0); // 00:00
    delay(10);

    updateBrightness();
    showIntro();
}

void setClock(byte hr, byte min)
{
    tmElements_t tm;
    tm.Hour = hr;
    tm.Minute = min;
    tm.Second = 0;
    tm.Day = 1;
    tm.Month = 1;
    tm.Year = 1;
    RTC.write(tm);
}

void loop()
{
    if (!settingTime)
    {
        if (reloadTime)
        {
            if (language == FRENCH)
            {
                readTimeFr();
            }
            else
            {
                readTime();
            }
            reloadTime = false;
        }
        if (!paused && nextPauseIsAt < millis() && !buttonPressed)
        {
            paused = true;
            watchFace.clearLeds();
            watchFace.drawLeds();
        }
        if (reloadTime || (nextAnimationIsAt < millis() && !buttonPressed))
        {
            int res = animation->draw();
            watchFace.drawLeds();
            if (res > 0)
            {
                nextAnimationIsAt = millis() + res + PAUSE;
                nextPauseIsAt = millis() + res;
                paused = false;
            }
            else
            {
                reloadTime = true;
                sleep();
            }
        }

        checkButtons();
    }
    else
    {
        switch (currentSetting)
        {
        case SETTING_LANGUAGE:
            watchFace.updateLeds(0, 0, false, 0, language, 0, true);
            break;
        case SETTING_FACE:
            watchFace.updateLeds(0, 0, true, 0, 0, _face, 0);
            break;
        case SETTING_HOURS:
            watchFace.updateLeds(0, 0, true, 0, _hours, 0, 0);
            break;
        case SETTING_PASTTO:
            watchFace.updateLeds(_pastTo<0, _pastTo> 0, 0, true, 0, 0, 0);
            break;
        case SETTING_MINUTES:
            watchFace.updateLeds(_pastTo == -1, _pastTo == 1, 0, true, _minutes, 0, 0);
            break;
        case SETTING_PM:
            watchFace.updateLeds(0, 0, 0, 0, 0, 0, _pm);
            break;
        }
        checkButtonWhileSetting();
        watchFace.drawLeds();
    }

    if (millis() > lastTimeBrightnessCheck + BRIGHTNESS_CHECK_INTERVAL)
    {
        updateBrightness();
        lastTimeBrightnessCheck = millis();
    }
}

/*
 * This is called when the interrupt occurs - button pressed, but we don't need to do anything in it
 */

ISR(PCINT0_vect)
{
}

/* 
 *  Doing sleep
 *  
 *  The sleep is interrupted by even shortest pressing MODE,
 *  but then there is a check, wheter the button is pressed for longer time to prevent unwanted waking-ups
 *  
 */
void sleep()
{
    watchFace.clearLeds();
    watchFace.drawLeds();
    digitalWrite(PIN_ENABLE, false);
    digitalWrite(PIN_LED_LATCH, true);
    digitalWrite(PIN_LED_DATA, true);
    digitalWrite(PIN_LED_CLOCK, true);
    digitalWrite(PIN_LED_PWM, true);

    delay(100);
    GIMSK |= _BV(PCIE0);                 
    PCMSK0 |= _BV(PCINT5);               
    ADCSRA &= ~_BV(ADEN);                
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
    sleep_enable();                      
    sei();                               
    sleep_cpu();                         

    // NOW WAKING UP
    cli();                  
    PCMSK0 &= ~_BV(PCINT5); 
    sleep_disable();        
    ADCSRA |= _BV(ADEN);    
    sei();                  

    nextAnimationIsAt = 0;
    analogWrite(PIN_LED_PWM, 254);
    digitalWrite(PIN_LED_LATCH, false);
    delay(10);
    digitalWrite(PIN_ENABLE, true);
    watchFace.clearLeds();
    watchFace.drawLeds();
    delay(PAUSE);
    lastTimeBrightnessCheck = millis();
}

void checkButtons()
{

    if (!digitalRead(PIN_BUTTON) && !buttonPressed)
    {
        buttonPressed = true;
        lastTimeButton = millis();
    }

    if (!digitalRead(PIN_BUTTON) && buttonPressed)
    {
        if (millis() - lastTimeButton > XLONG_PRESS)
        {
            _face = 1;
            _hours = 1;
            _minutes = 1;
            _pastTo = 1;
            _pm = true;
            nextAnimationIsAt = 0;
            reloadTime = true;
            settingTime = true;
            ignoreFirstSettingClick = true;
            currentSetting = 0;
        }
    }
    if (digitalRead(PIN_BUTTON) && buttonPressed)
    {
        if (millis() - lastTimeButton > DEBOUNCE_PRESS)
        {
            reloadTime = true;
            buttonPressed = false;
            nextAnimationIsAt = 0;
            watchFace.clearLeds();
            watchFace.drawLeds();
            smashCheck();
            delay(40);
        }
    }
}

void smashCheck()
{
    if (millis() < smashingStarted + SMASH_INTERVAL)
    {
        smashingCount++;
        if (smashingCount >= SMASH_COUNT)
        {
            smashingCount = 0;
            watchFace.clearLeds();
            watchFace.drawLeds();
            delay(PAUSE * 2);
            showIntro();
        }
    }
    else
    {
        smashingCount = 0;
        smashingStarted = millis();
    }
}

void checkButtonWhileSetting()
{
    if (!digitalRead(PIN_BUTTON) && !buttonPressed)
    {
        buttonPressed = true;
        lastTimeButton = millis();
    }
    if (!digitalRead(PIN_BUTTON) && buttonPressed)
    {
        if (millis() - lastTimeButton > LONG_PRESS)
        {
            if (!ignoreFirstSettingClick)
            {
                settingsButtonLongPress();
            }
            ignoreFirstSettingClick = true;
            buttonPressed = false;
        }
    }
    if (digitalRead(PIN_BUTTON) && buttonPressed)
    {
        if (millis() - lastTimeButton > DEBOUNCE_PRESS && millis() - lastTimeButton < LONG_PRESS)
        {
            if (!ignoreFirstSettingClick)
            {
                settingsButtonClick();
            }
            ignoreFirstSettingClick = false;
            buttonPressed = false;
        }
    }
}

void settingsButtonClick()
{
    switch (currentSetting)
    {
    case SETTING_LANGUAGE:
        language++;
        if (language > LANGUAGES_COUNT)
        {
            language = 1;
        }
        break;
    case SETTING_FACE:
        _face++;
        if (_face > 4)
        {
            _face = 1;
        }
        break;

    case SETTING_HOURS:
        _hours++;
        if (_hours > 12)
        {
            _hours = 1;
        }
        break;

    case SETTING_PASTTO:
        _pastTo++;
        if (_pastTo > 1)
        {
            _pastTo = -1;
        }
        break;

    case SETTING_MINUTES:
        _minutes++;
        if (_minutes > 12)
        {
            _minutes = 1;
        }
        break;

    case SETTING_PM:
        _pm = !_pm;
        break;
    }
}

void settingsButtonLongPress()
{
    currentSetting++;
    if (_pastTo == 0 && currentSetting == SETTING_MINUTES)
    {
        currentSetting++;
    }
    if (currentSetting == SETTING_STEPS)
    {
        finishSetTime();
        settingTime = false;
    }
}

/**
 * Reads brightness value from the sensor and updates PWM duty cycle
 */
void updateBrightness()
{
    int val = analogRead(PIN_LIGHT_SENSOR);
    val = val * 4;
    if (val > 1023)
    {
        val = 1023;
    }
    byte bright = map(val, 0, 1023, 0, 255);
    analogWrite(PIN_LED_PWM, 255 - bright);
}

/**
 * Finishes time setting
 * Quite complicated, since each language works differently
 */
void finishSetTime()
{
    int mn = (_face - 1) * 15 + _minutes * _pastTo;
    byte hr = _hours;

    if (language == CZECH || language == HUNGARIAN)
    {
        if (_face > 1 || mn < 0)
        {
            hr--;
        }
    }
    else if (language == ENGLISH || language == FRENCH)
    {
        if (_face > 3 || mn < 0)
        {
            hr--;
        }
    }
    else if (language == GERMAN || language == POLISH)
    {
        if (_face > 2 || mn < 0)
        {
            hr--;
        }
    }

    if (mn < 0)
    {
        mn += 60;
    }

    if (_pm)
    {
        hr += 12;
    }
    if (hr == 24)
    {
        hr = 0;
    }

    delay(10);
    setClock(hr, mn);
    delay(10);
}

/**
 * Reads time from RTC and displays it
 */
void readTime()
{
    tmElements_t t;
    RTC.read(t);

    int hh = t.Hour;
    int mm = t.Minute;

    byte aftercoef = 0; // how "quarter after" modifies hours
    byte halfcoef = 0; // how "half past" modifies hours
    byte tocoef = 0; // "how quarter to" modifies hours

    if (language == CZECH || language == HUNGARIAN)
    {
        aftercoef = 1;
        halfcoef = 1;
        tocoef = 1;
    }
    else if (language == ENGLISH)
    {
        aftercoef = 0;
        halfcoef = 0;
        tocoef = 1;
    }
    else if (language == GERMAN || language == POLISH)
    {
        aftercoef = 0;
        halfcoef = 1;
        tocoef = 1;
    }

    byte minutes = mm;
    byte hours = hh % 12;
    bool pm = (hh >= 12);

    delete animation;

    if (minutes == 0)
    {
        animation = new AnimationExact();
        animation->setTime(hours, 0, 1, pm);
    }
    else if (minutes < 11)
    {
        animation = new AnimationPast();
        animation->setTime(hours, minutes, 1, pm);
    }
    else if (minutes < 15)
    {
        animation = new AnimationTo();
        animation->setTime(hours + aftercoef, 15 - minutes, 2, pm);
    }
    else if (minutes == 15)
    {
        animation = new AnimationExact();
        animation->setTime(hours + aftercoef, 0, 2, pm);
    }
    else if (minutes < 20)
    {
        animation = new AnimationPast();
        animation->setTime(hours + aftercoef, minutes - 15, 2, pm);
    }
    else if (minutes < 30)
    {
        animation = new AnimationTo();
        animation->setTime(hours + halfcoef, 30 - minutes, 3, pm);
    }
    else if (minutes == 30)
    {
        animation = new AnimationExact();
        animation->setTime(hours + halfcoef, 0, 3, pm);
    }
    else if (minutes < 41)
    {
        animation = new AnimationPast();
        animation->setTime(hours + halfcoef, minutes - 30, 3, pm);
    }
    else if (minutes < 45)
    {
        animation = new AnimationTo();
        animation->setTime(hours + tocoef, 45 - minutes, 4, pm);
    }
    else if (minutes == 45)
    {
        animation = new AnimationExact();
        animation->setTime(hours + tocoef, 0, 4, pm);
    }
    else if (minutes < 50)
    {
        animation = new AnimationPast();
        animation->setTime(hours + tocoef, minutes - 45, 4, pm);
    }
    else
    {
        animation = new AnimationTo();
        animation->setTime(hours + 1, 60 - minutes, 1, pm);
    }

    animation->setup();
}

/**
 * Special time read for French
 */
void readTimeFr()
{
    tmElements_t t;
    RTC.read(t);
    animation = new AnimationFrench();
    animation->setTime(t.Hour % 12, t.Minute, 0, t.Hour > 12);
    animation->setup();
}

/**
 * Shows intro animation which testes all diodes
 */
void showIntro()
{

    animation = new AnimationIntro();
    animation->setup();
    int waitFor = 0;
    do
    {
        waitFor = animation->draw();
        watchFace.drawLeds();
        delay(waitFor);
    } while (waitFor != 0);
    watchFace.clearLeds();
    watchFace.drawLeds();
    delay(1000);
}