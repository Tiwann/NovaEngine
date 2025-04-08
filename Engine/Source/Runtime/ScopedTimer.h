#pragma once
#include "Containers/MulticastDelegate.h"
#include <GLFW/glfw3.h>

namespace Nova
{
    class ScopedTimer
    {
    public:
        using FinishedDelegate = MulticastDelegate<void(f64 Duration)>;
        FinishedDelegate FinishedEvent;
        
        ScopedTimer(const FinishedDelegate::DelegateType& OnFinished)
        {
            FinishedEvent.Bind(OnFinished);
            m_Start = glfwGetTime();
        }
        
        ~ScopedTimer()
        {
            m_End = glfwGetTime();
            const f64 Duration = m_End - m_Start;
            FinishedEvent.BroadcastChecked(Duration);
            FinishedEvent.ClearAll();
        }
    private:
        f64 m_Start;
        f64 m_End;
    };
}
