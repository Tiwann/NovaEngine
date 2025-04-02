#pragma once
#include "Containers/MulticastDelegate.h"

namespace Nova
{
    enum class TweenEase
    {
        Linear,
        QuadraticIn,
        QuadraticOut,
        QuadraticInOut,
        CubicIn,
        CubicOut,
        CubicInOut,
        ExponentialIn,
        ExponentialOut,
        ExponentialInOut,
        BackIn,
        BackOut,
        BackInOut,
    };

    static f32 Ease(TweenEase Ease, f32 Time)
    {
        switch (Ease)
        {
        case TweenEase::Linear: return Time;
        case TweenEase::QuadraticIn: return Time * Time;
        case TweenEase::QuadraticOut: return 1.0f - (1.0f - Time) * (1.0f - Time);
        case TweenEase::QuadraticInOut: return Time < 0.5
                                                   ? 2 * Time * Time
                                                   : 1 - Math::Pow(-2.0f * Time + 2.0f, 2.0f) / 2.0f;
        case TweenEase::CubicIn: return Time * Time * Time;
        case TweenEase::CubicOut: return 1.0f - (1.0f - Time) * (1.0f - Time) * (1.0f - Time);
        case TweenEase::CubicInOut: return Time < 0.5
                                               ? 4 * Time * Time * Time
                                               : 1 - Math::Pow(-2.0f * Time + 2.0f, 3.0f) / 2.0f;
        case TweenEase::ExponentialIn: return Math::AreSame(Time, 0.0f) ? 0.0f : Math::Pow(2.0f, 10.0f * Time - 10.0f);
        case TweenEase::ExponentialOut: return Math::AreSame(Time, 1.0f) ? 1.0f : 1.0f - Math::Pow(2.0f, -10.0f * Time);
        case TweenEase::ExponentialInOut: return Math::AreSame(Time, 0.0f)
                                                     ? 0.0f
                                                     : Math::AreSame(Time, 1.0f)
                                                     ? 1.0f
                                                     : Time < 0.5f
                                                     ? Math::Pow(2.0f, 20.0f * Time - 10.0f) / 2.0f
                                                     : (2.0f - Math::Pow(2.0f, -20.0f * Time + 10.0f)) / 2.0f;
        case TweenEase::BackIn:
            {
                constexpr f32 C1 = 1.70158f;
                constexpr f32 C3 = C1 + 1.0f;
                return C3 * Time * Time * Time - C1 * Time * Time;
            }
        case TweenEase::BackOut:
            {
                constexpr f32 C1 = 1.70158f;
                constexpr f32 C3 = C1 + 1.0f;
                return 1 + C3 * Math::Pow(Time - 1.0f, 3.0f) + C1 * Math::Pow(Time - 1.0f, 2.0f);
            }
        case TweenEase::BackInOut:
            {
                constexpr f32 C1 = 1.70158f;
                constexpr f32 C2 = C1 * 1.525f;
                return Time < 0.5f
                           ? (Math::Pow(2.0f * Time, 2.0f) * ((C2 + 1.0f) * 2.0f * Time - C2)) / 2.0f
                           : (Math::Pow(2.0f * Time - 2.0f, 2.0f) * ((C2 + 1.0f) * (Time * 2.0f - 2.0f) + C2) + 2.0f) /
                           2.0f;
            }
        }
        return 0.0f;
    }

    class TweenManager;
    
    class Tweener
    {
    public:
        using TweenDelegate = MulticastDelegate<void()>;

        Tweener(TweenManager* Manager) : m_Manager(Manager)
        {
            
        }

        virtual ~Tweener()
        {
            m_OnStartEvent.ClearAll();
            m_OnEndEvent.ClearAll();
        }
        
        virtual void Update(float Delta) = 0;
        
        Tweener& OnStart(const TweenDelegate::DelegateType& OnStart)
        {
            m_OnStartEvent.ClearAll();
            m_OnStartEvent.Bind(OnStart);
            return *this;
        }
        
        Tweener& OnEnd(const TweenDelegate::DelegateType& OnStart)
        {
            m_OnEndEvent.ClearAll();
            m_OnEndEvent.Bind(OnStart);
            return *this;
        }

        Tweener& WithDuration(f32 Duration)
        {
            m_Duration = Duration;
            return *this;
        }

        Tweener& WithEase(TweenEase Ease)
        {
            m_Ease = Ease;
            return *this;
        }

        bool IsFinished() const
        {
            return m_Finished;
        }
    protected:
        f32           m_Time = 0.0f;
        f32           m_Duration = 0.0f;
        TweenDelegate m_OnStartEvent;
        TweenDelegate m_OnEndEvent;
        TweenEase     m_Ease = TweenEase::Linear;
        TweenManager* m_Manager = nullptr;
        bool m_Finished = false;
    };

    template<typename T>
    class TweenerValue : public Tweener
    {
    public:
        using Getter = Function<T()>;
        using Setter = Function<void(const T&)>;
        
        TweenerValue(TweenManager* Manager, Getter Getter, Setter Setter)
        : Tweener(Manager), m_Getter(std::move(Getter)), m_Setter(std::move(Setter))
        {
            m_StartValue = m_Getter();
        }

        void Update(float Delta) override
        {
            if (m_Finished) return;
            
            if (m_OnStartEvent.IsBound()) m_OnStartEvent.Broadcast();
            m_Time += Delta;
            m_Time = Math::Clamp(m_Time, 0.0f, m_Duration);
            const f32 RemappedTime = m_Time / m_Duration;

            
            m_Setter(Math::Lerp<T>(m_StartValue, m_EndValue, Ease(m_Ease, RemappedTime)));
            
            if (m_Time >= m_Duration)
            {
                m_Finished = true;
                m_Setter(m_EndValue);
                if (m_OnEndEvent.IsBound()) m_OnEndEvent.Broadcast();
            }
        }
        
        
        TweenerValue& ChangeStartValue(const T& StartValue)
        {
            m_StartValue = StartValue;
            return *this;
        }
        
        TweenerValue& ChangeEndValue(const T& EndValue)
        {
            m_EndValue = EndValue;
            return *this;
        }
    protected:
        T m_StartValue = T();
        T m_EndValue = T();
        Getter m_Getter;
        Setter m_Setter;
    };

    template<typename T>
    class TweenerRef : public Tweener
    {
    public:
        using Getter = Function<T&()>;
        using Setter = Function<void(const T&)>;
        
        TweenerRef(TweenManager* Manager, T& Ref)
        : Tweener(Manager), m_Ref(Ref)
        {
            m_StartValue = m_Ref;
        }
        
        TweenerRef& ChangeStartValue(const T& StartValue)
        {
            m_StartValue = StartValue;
            return *this;
        }
        
        TweenerRef& ChangeEndValue(const T& EndValue)
        {
            m_EndValue = EndValue;
            return *this;
        }

        void Update(float Delta) override
        {
            if (m_Finished) return;
            
            if (m_OnStartEvent.IsBound()) m_OnStartEvent.Broadcast();
            m_Time += Delta;
            m_Time = Math::Clamp(m_Time, 0.0f, m_Duration);
            const f32 RemappedTime = m_Time / m_Duration;
            
            m_Ref = Math::Lerp<T>(m_StartValue, m_EndValue, Ease(m_Ease, RemappedTime));
            
            if (m_Time >= m_Duration)
            {
                m_Finished = true;
                m_Ref = (m_EndValue);
                if (m_OnEndEvent.IsBound()) m_OnEndEvent.Broadcast();
            }
        }

    protected:
        T m_StartValue = T();
        T m_EndValue = T();
        T& m_Ref;
    };

    
}