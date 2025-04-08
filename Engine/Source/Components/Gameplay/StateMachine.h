#pragma once
#include "State.h"
#include "Runtime/Component.h"
#include "Core/LogCategory.h"
#include "Core/Log.h"
#include "Core/LogVerbosity.h"
#include "Core/TypeTraits.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(StateMachine, "STATE MACHINE")

namespace Nova
{
    template<typename StateBase> requires IsBaseOfValue<State, StateBase>
    class StateMachine : public Component
    {
    protected:
        StateMachine(Entity* Owner, const String& Name = "StateMachine") : Component(Owner, Name) {}
    public:
        void ChangeState(StateBase* NewState)
        {
            m_CurrentState->OnExit();
            m_CurrentState = NewState;
            m_CurrentState->OnEnter();
        }
        
        const StateBase* GetCurrentState() const
        {
            return m_CurrentState;
        }

        void SetCurrentState(const StateBase& NewState)
        {
            m_CurrentState = &NewState;
        }

        void OnStart() override
        {
            m_CurrentState->OnEnter();
        }

        void OnUpdate(f32 Delta) override
        {
            if(!m_CurrentState)
            {
                NOVA_LOG(StateMachine, Verbosity::Error, "Current state is null, cannot update state machine");
                return;
            }

            m_CurrentState->OnUpdate(Delta);
        }

        void OnDestroy() override
        {
            m_CurrentState->OnExit();
        }

    protected:
        StateBase* m_CurrentState = nullptr;
    };
}
