#include "Timer.h"

namespace Nova
{
    Timer::Timer(const float duration, const bool loop, const FinishedDelegate::DelegateType& callback)
        : m_Duration(duration), m_Loop(loop)
    {
        finishedEvent.Bind(callback);
    }

    Timer::~Timer()
    {
        finishedEvent.ClearAll();
    }

    void Timer::Reset()
    {
        m_Time = 0.0f;
    }

    void Timer::Start()
    {
        m_Started = true;
    }

    void Timer::Stop()
    {
        m_Started = false;
    }

    void Timer::Update(const float deltaTime)
    {
        if(!m_Started) return;
        
        m_Time += deltaTime;
        if(m_Time >= m_Duration)
        {
            m_Time = 0.0f;
            finishedEvent.BroadcastChecked();
            if(!m_Loop) m_Started = false;
        }
    }

    void Timer::SetLoop(const bool loop)
    {
        m_Loop = loop;
    }

    void Timer::SetDuration(const float duration)
    {
        m_Duration = duration;
    }
}
