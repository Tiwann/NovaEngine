#include "ScopedTimer.h"
#include "Time.h"

namespace Nova
{
    ScopedTimer::ScopedTimer(const Function<void(double elapsed)>& onFinished) : m_StartTime(Time::Get()), m_OnFinished(onFinished)
    {
    }

    ScopedTimer::~ScopedTimer()
    {
        const double elapsedTime = Time::Get() - m_StartTime;
        if (m_OnFinished)
            m_OnFinished(elapsedTime);
    }
}
