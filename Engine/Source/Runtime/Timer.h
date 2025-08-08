#pragma once
#include "Containers/MulticastDelegate.h"

namespace Nova
{
    class Timer
    {
    public:
        using FinishedDelegate = MulticastDelegate<void()>;

        Timer() = default;
        Timer(float duration, bool loop, const FinishedDelegate::DelegateType& callback);
        ~Timer();

        void Reset();
        void Start();
        void Stop();
        void Update(float deltaTime);

        void SetLoop(bool loop);
        void SetDuration(float duration);

        FinishedDelegate finishedEvent;
    private:
        float m_Time = 0.0f;
        float m_Duration = 0.0f;
        bool m_Started = false;
        bool m_Loop = false;
    };
}
