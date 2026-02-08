#include "Locale.hpp"
#include "App.hpp"

Locale::TimeCoefficients Locale::GetTimeCoefficients() const
{
    auto language = App::settings.GetLanguage();
    switch (language)
    {
    case Language::CZECH:
    case Language::HUNGARIAN:
        return {.first_quarter = 1, .half = 1, .third_quarter = 1};
    case Language::GERMAN:
    case Language::POLISH:
    case Language::NORWEGIAN:
        return {.first_quarter = 0, .half = 1, .third_quarter = 1};
    case Language::ENGLISH:
    default:
        return {.first_quarter = 0, .half = 0, .third_quarter = 1};
    }
}

int32_t Locale::HourOffsetWhenSaving(Display::ClockFace face, int minutes) const
{
    // Always decrement if minutes are negative (going to previous hour)
    if (minutes < 0)
    {
        return -1;
    }

    // Use time coefficients to determine when to decrement based on face position
    TimeCoefficients coefficients = GetTimeCoefficients();
    switch (face)
    {
    case Display::ClockFace::RIGHT:
        return -coefficients.first_quarter;
    case Display::ClockFace::DOWN:
        return -coefficients.half;
    case Display::ClockFace::LEFT:
        return -coefficients.third_quarter;
    case Display::ClockFace::UP:
    default:
        return 0; // UP face never decrements
    }
}

FrameSequence Locale::GetSequence(TimeType time_type) const
{
    switch (time_type)
    {
    case TimeType::EXACT:
        // Exact time is the same for all languages
        return {ClockFrame::CLOCKFACE, ClockFrame::HOURS_NUM, ClockFrame::HOURS, ClockFrame::PM};

    case TimeType::PAST:
        switch (App::settings.GetLanguage())
        {
        case Language::HUNGARIAN:
            return {ClockFrame::AFTER, ClockFrame::MINUTES_NUM, ClockFrame::MINUTES,
                    ClockFrame::CLOCKFACE, ClockFrame::HOURS_NUM, ClockFrame::HOURS, ClockFrame::PM};
        case Language::NORWEGIAN:
            return {ClockFrame::MINUTES_NUM, ClockFrame::MINUTES, ClockFrame::AFTER,
                    ClockFrame::CLOCKFACE, ClockFrame::HOURS_NUM, ClockFrame::HOURS, ClockFrame::PM};
        case Language::CZECH:
        case Language::POLISH:
        case Language::ENGLISH:
        case Language::GERMAN:
        default:
            return {ClockFrame::CLOCKFACE, ClockFrame::HOURS_NUM, ClockFrame::HOURS,
                    ClockFrame::AFTER, ClockFrame::MINUTES_NUM, ClockFrame::MINUTES, ClockFrame::PM};
        }

    case TimeType::TO:
        switch (App::settings.GetLanguage())
        {
        case Language::CZECH:
        case Language::POLISH:
            return {ClockFrame::BEFORE, ClockFrame::MINUTES_NUM, ClockFrame::MINUTES,
                    ClockFrame::CLOCKFACE, ClockFrame::HOURS_NUM, ClockFrame::HOURS, ClockFrame::PM};
        case Language::HUNGARIAN:
        case Language::ENGLISH:
        case Language::GERMAN:
        case Language::NORWEGIAN:
        default:
            return {ClockFrame::MINUTES_NUM, ClockFrame::MINUTES, ClockFrame::BEFORE,
                    ClockFrame::CLOCKFACE, ClockFrame::HOURS_NUM, ClockFrame::HOURS, ClockFrame::PM};
        }

    default:
        return {};
    }
}

Locale::TimeParameters Locale::FixTime(const TimeParameters &params) const
{
    TimeParameters fixed_params = params;

    // Wrap hours around to 0-11
    fixed_params.hours = fixed_params.hours % 12;

    // Display 0 as 12
    if (fixed_params.hours == 0)
    {
        fixed_params.hours = 12;
    }

    return fixed_params;
}

Locale::TimeParameters Locale::ProcessTime(const uint32_t hours, const uint32_t minutes) const
{
    TimeCoefficients coefficients = GetTimeCoefficients();
    uint32_t hours12 = hours % 12;
    bool pm = ((hours % 24) >= 12);

    // Determine animation type and parameters based on minutes
    if (minutes == 0)
    {
        return FixTime({TimeType::EXACT, hours12, 0, Display::ClockFace::UP, pm});
    }
    else if (minutes < 11)
    {
        return FixTime({TimeType::PAST, hours12, minutes, Display::ClockFace::UP, pm});
    }
    else if (minutes < 15)
    {
        return FixTime({TimeType::TO, hours12 + coefficients.first_quarter, 15 - minutes, Display::ClockFace::RIGHT, pm});
    }
    else if (minutes == 15)
    {
        return FixTime({TimeType::EXACT, hours12 + coefficients.first_quarter, 0, Display::ClockFace::RIGHT, pm});
    }
    else if (minutes < 20)
    {
        return FixTime({TimeType::PAST, hours12 + coefficients.first_quarter, minutes - 15, Display::ClockFace::RIGHT, pm});
    }
    else if (minutes < 30)
    {
        return FixTime({TimeType::TO, hours12 + coefficients.half, 30 - minutes, Display::ClockFace::DOWN, pm});
    }
    else if (minutes == 30)
    {
        return FixTime({TimeType::EXACT, hours12 + coefficients.half, 0, Display::ClockFace::DOWN, pm});
    }
    else if (minutes < 41)
    {
        return FixTime({TimeType::PAST, hours12 + coefficients.half, minutes - 30, Display::ClockFace::DOWN, pm});
    }
    else if (minutes < 45)
    {
        return FixTime({TimeType::TO, hours12 + coefficients.third_quarter, 45 - minutes, Display::ClockFace::LEFT, pm});
    }
    else if (minutes == 45)
    {
        return FixTime({TimeType::EXACT, hours12 + coefficients.third_quarter, 0, Display::ClockFace::LEFT, pm});
    }
    else if (minutes < 50)
    {
        return FixTime({TimeType::PAST, hours12 + coefficients.third_quarter, minutes - 45, Display::ClockFace::LEFT, pm});
    }
    else
    {
        return FixTime({TimeType::TO, hours12 + 1, 60 - minutes, Display::ClockFace::UP, pm});
    }
}
