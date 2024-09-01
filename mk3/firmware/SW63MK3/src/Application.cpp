#include "Application.h"
#include "Config.h"

namespace SW63
{

    void Application::Init(voidFuncPtr button_callback)
    {
        Config::SetSpeed(0);
        Config::SetLanguage(LANG_CZECH);
        
        hw_.Init(button_callback);
        
        // Prepare animations
        for(int i = 0; i < AT_COUNT; i++)
        {
            animations_[i]->Init(&hw_);
        }

        // Set intro animation
        SetAnimation(AT_INTRO);
    }

    void Application::Process()
    {
        hw_.AutoBrightness();
        uint32_t wait = current_animation_->Process();
        hw_.TransferLeds();
        if (wait > 0)
        {
            delay(wait);
        }
        else
        {
            current_animation_->Reset();
            hw_.Sleep();
        }
    }

    void Application::ButtonCallback()
    {
    }

    void Application::SetAnimation(AnimationType type)
    {
        current_animation_ = animations_[type];
        current_animation_->Reset();
    }
}