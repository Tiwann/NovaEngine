#include "Timer.h"

namespace Nova
{
    Timer::Timer(f32 Duration, bool Loop, const FinishedDelegate::DelegateType& Callback)
        : m_Started(false), m_Loop(Loop), m_Duration(Duration)
    {
        FinishedEvent.Bind(Callback);
    }

    Timer::~Timer()
    {
        FinishedEvent.ClearAll();
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

    void Timer::Update(f32 Delta)
    {
        if(!m_Started) return;
        
        m_Time += Delta;
        if(m_Time >= m_Duration)
        {
            m_Time = 0.0f;
            FinishedEvent.BroadcastChecked();
            if(!m_Loop) m_Started = false;
        }
    }

    void Timer::SetLoop(bool Loop)
    {
        m_Loop = Loop;
    }

    void Timer::SetDuration(f32 Duration)
    {
        m_Duration = Duration;
    }
}
