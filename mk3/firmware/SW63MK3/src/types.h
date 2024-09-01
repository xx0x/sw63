#ifndef SW63_TYPES_H
#define SW63_TYPES_H

namespace SW63
{
    enum Language
    {
        LANG_CZECH,
        LANG_ENGLISH,
        LANG_GERMAN,
        LANG_POLISH,
        LANG_HUNGARIAN,
        LANG_COUNT
    };

    enum ClockFace
    {
        CF_NONE,
        CF_UP,
        CF_RIGHT,
        CF_DOWN,
        CF_LEFT
    };

    enum AnimationType
    {
        AT_INTRO,
        AT_EXACT,
        AT_TO,
        AT_PAST,
        AT_COUNT
    };
}

#endif