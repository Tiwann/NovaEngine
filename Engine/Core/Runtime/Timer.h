#pragma once
#include "Containers/MulticastDelegate.h"
#include <CoreExport.h>

namespace Nova
{
    class CORE_API Timer
    {
    public:
        using FinishedDelegate = MulticastDelegate<void()>;
        Timer() = default;
        Timer(f32 Duration, bool Loop, const FinishedDelegate::DelegateType& Callback);
        ~Timer();

        void Reset();
        void Start();
        void Stop();
        void Update(f32 Delta);

        void SetLoop(bool Loop);
        void SetDuration(f32 Duration);

        FinishedDelegate FinishedEvent;
    private:
        f32 m_Time{0.0f};
        bool m_Started{false};
        bool m_Loop{false};
        f32 m_Duration{0.0f};
    };
}
