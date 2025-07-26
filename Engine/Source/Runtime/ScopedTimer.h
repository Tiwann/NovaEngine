#pragma once
#include "Containers/Function.h"

namespace Nova
{
    class ScopedTimer
    {
    public:
        explicit ScopedTimer(const Function<void(double elapsed)>& onFinished);
        ~ScopedTimer();
    private:
        double m_StartTime = 0.0;
        Function<void(double elapsed)> m_OnFinished = nullptr;
    };
}
