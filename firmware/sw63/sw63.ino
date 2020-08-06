// SW63 SPACEWATCH SOURCE CODE
// for MCU: ATtiny 84 (ATTinyCore) 8 MHz
// author: Vaclav Mach (vaclavmach@hotmail.cz, info@xx0x.cz)
// project homepage: www.xx0x.cz

#include <avr/interrupt.h>
#include <avr/sleep.h>

#include <DS3232RTC.h> // https://github.com/JChristensen/DS3232RTC
#include <TinyWireM.h>
#include <TimeLib.h>

#define SW63_1_0 1
#define SW63_2_0 2
#define SW63_VERSION SW63_2_0

// pin definitions - see schematic
#define PIN_ENABLE 3
#define PIN_LED_DATA 2
#define PIN_LED_CLOCK 1
#define PIN_LED_LATCH 0
#define PIN_LED_PWM 8
#define PIN_LIGHT_SENSOR A7
#define PIN_BUTTON 5

// modes
#define MODE_DISPLAY 0
#define MODE_SETTING 1
#define MODE_SECRET 2
byte mode = MODE_DISPLAY;

// display settings
#define SPEED_COUNT 6
byte currentSpeed = 0;
unsigned int speedHoldDigits[SPEED_COUNT] = {1000, 800, 600, 400, 300, 150};
unsigned int speedHold[SPEED_COUNT] = {800, 640, 480, 320, 240, 120};
unsigned int speedPause[SPEED_COUNT] = {150, 120, 90, 60, 45, 23};
unsigned int speedHoldCompressed[SPEED_COUNT] = {2000, 1600, 1200, 800, 600, 300};
#define COMPRESSED compressedActive
bool compressedActive = false;
#define SAVER saverActive
bool saverActive = false;

// how long in ms LEDs should stay on
#define HOLD_COMPRESSED speedHoldCompressed[currentSpeed]
#define HOLD_DIGITS speedHoldDigits[currentSpeed]
#define HOLD speedHold[currentSpeed]
#define PAUSE speedPause[currentSpeed]
#define SKIP 1

// language defintions
#define LANGUAGES_COUNT 5
#define CZECH 1
#define ENGLISH 2
#define GERMAN 3
#define POLISH 4
#define HUNGARIAN 5

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
Animation *animation;

// button and timings
bool buttonPressed = false;
unsigned long lastTimeButton = 0;
unsigned long smashingStarted = 0;
byte smashingCount = 0;
bool ignoreNextPress = false;
bool ignoreNextLongPress = false;
#define SMASH_INTERVAL 2000
#define SMASH_COUNT 3
#define LONG_PRESS 1500
#define XLONG_PRESS 4000
#define DEBOUNCE_PRESS 10
#define BUTTON_PRESSED (!digitalRead(PIN_BUTTON))
#define SETTINGS_TIMEOUT 15000

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
byte currentSetting = 0;

// variables for storing when setting time
byte _face = 1;
byte _hours = 1;
byte _minutes = 1;
int _pastTo = 1;
bool _pm = true;

// secret menu
bool waitForSecretMenu = false;
bool secretOpened = false;
#define SECRET_ITEMS 4
#define SECRET_SPEED 0
#define SECRET_COMPRESSED 1
#define SECRET_SAVER 2

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
    setClock(0, 0);

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

void displayTimeLoop()
{
    if (reloadTime)
    {
        readTime();
        reloadTime = false;
    }
    if (!paused && nextPauseIsAt < millis() && (!buttonPressed || ignoreNextPress))
    {
        paused = true;
        watchFace.clearLeds();
        watchFace.drawLeds();
    }
    if (reloadTime || (nextAnimationIsAt < millis() && (!buttonPressed || ignoreNextPress)))
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
}

void settingTimeLoop()
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
    watchFace.drawLeds();
}

void secretMenuLoop()
{
    byte val = 0;
    if (secretOpened)
    {
        switch (currentSetting)
        {
        case SECRET_SPEED:
            val = currentSpeed + 1;
            break;
        case SECRET_COMPRESSED:
            val = compressedActive ? 2 : 1;
            break;
        case SECRET_SAVER:
            val = saverActive ? 2 : 1;
            break;
        }
    }
    watchFace.updateLeds(currentSetting == 3, currentSetting == 2, currentSetting == 1, currentSetting == 0, val, 0, secretOpened);
    watchFace.drawLeds();
}

void checkForMenuExit()
{
    if (millis() - lastTimeButton > SETTINGS_TIMEOUT)
    {
        setMode(MODE_DISPLAY);
        return;
    }
}

void loop()
{
    switch (mode)
    {
    case MODE_DISPLAY:
        displayTimeLoop();
        break;
    case MODE_SETTING:
        settingTimeLoop();
        checkForMenuExit();
        break;
    case MODE_SECRET:
        secretMenuLoop();
        checkForMenuExit();
        break;
    }

    if (millis() > lastTimeBrightnessCheck + BRIGHTNESS_CHECK_INTERVAL)
    {
        updateBrightness();
        lastTimeBrightnessCheck = millis();
    }

    checkButtons();
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

bool shortButtonPress()
{
    switch (mode)
    {
    case MODE_DISPLAY:
        reloadTime = true;
        nextAnimationIsAt = 0;
        watchFace.clearLeds();
        watchFace.drawLeds();
        smashCheck();
        delay(40);
        return true;
    case MODE_SECRET:
        if (secretOpened)
        {
            switch (currentSetting)
            {
            case SECRET_SPEED:
                currentSpeed++;
                if (currentSpeed >= SPEED_COUNT)
                {
                    currentSpeed = 0;
                }
                break;
            case SECRET_COMPRESSED:
                compressedActive = !compressedActive;
                break;
            case SECRET_SAVER:
                saverActive = !saverActive;
                break;
            }
        }
        else
        {
            currentSetting++;
            if (currentSetting >= SECRET_ITEMS)
            {
                currentSetting = 0;
            }
        }
        delay(40);
        return true;
    case MODE_SETTING:
        settingsButtonClick();
        return true;
    }
    return false;
}

bool longButtonPress()
{
    switch (mode)
    {
    case MODE_SETTING:
        settingsButtonLongPress();
        return true;
    case MODE_SECRET:
        if (currentSetting == 3)
        {
            setMode(MODE_DISPLAY);
        }
        else
        {
            secretOpened = !secretOpened;
        }
        ignoreNextPress = true;
        return true;
    }
    return false;
}

bool xlongButtonPress()
{
    switch (mode)
    {
    case MODE_DISPLAY:
        setMode(MODE_SETTING);
        ignoreNextLongPress = true;
        return true;
    case MODE_SETTING:
        setMode(MODE_SECRET);
        return true;
    }
    return false;
}

void checkButtons()
{

    if (BUTTON_PRESSED && !buttonPressed)
    {
        buttonPressed = true;
        lastTimeButton = millis();
    }

    if (BUTTON_PRESSED && buttonPressed)
    {
        unsigned long diff = millis() - lastTimeButton;
        if (diff > LONG_PRESS && !ignoreNextLongPress)
        {
            buttonPressed = !longButtonPress();
        }
        if (diff > XLONG_PRESS)
        {
            buttonPressed = !xlongButtonPress();
        }
    }
    if (!BUTTON_PRESSED && buttonPressed)
    {
        if (millis() - lastTimeButton > DEBOUNCE_PRESS && millis() - lastTimeButton < LONG_PRESS)
        {
            if (!ignoreNextPress)
            {
                shortButtonPress();
            }
            ignoreNextPress = false;
            buttonPressed = false;
        }
        ignoreNextLongPress = false;
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
    ignoreNextPress = true;
    currentSetting++;
    if (_pastTo == 0 && currentSetting == SETTING_MINUTES)
    {
        currentSetting++;
    }
    if (currentSetting == SETTING_STEPS)
    {
        finishSetTime();
        setMode(MODE_DISPLAY);
    }
}

void setMode(byte newMode)
{
    _face = 1;
    _hours = 1;
    _minutes = 1;
    _pastTo = 1;
    _pm = true;
    nextAnimationIsAt = 0;
    reloadTime = true;
    currentSetting = 0;
    ignoreNextPress = true;
    mode = newMode;
}

/**
 * Reads brightness value from the sensor and updates PWM duty cycle
 */
void updateBrightness()
{
    int val = analogRead(PIN_LIGHT_SENSOR);
#if SW63_VERSION == SW63_2_0
    val = val * 2.5; // version 2.0 uses a different light sensor
#else
    val = val * 4;
#endif
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
    else if (language == ENGLISH)
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
    byte halfcoef = 0;  // how "half past" modifies hours
    byte tocoef = 0;    // "how quarter to" modifies hours

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
 * Shows intro animation which testes all diodes
 */
void showIntro()
{

    animation = new AnimationIntro();
    animation->setup();
    int waitFor = 0;
    do
    {
        waitFor = animation->drawRegular();
        watchFace.drawLeds();
        delay(waitFor);
    } while (waitFor != 0);
    watchFace.clearLeds();
    watchFace.drawLeds();
    delay(1000);
}