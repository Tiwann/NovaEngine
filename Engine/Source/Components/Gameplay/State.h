#pragma once
#include "Core/NumericTypes.h"
#include "Core/TypeTraits.h"

namespace Nova
{
    class StateMachine;
    
    class State
    {
    public:
        State(StateMachine* InStateMachine) : m_StateMachine(InStateMachine){}
        virtual ~State() = default;
        virtual void OnEnter(){}
        virtual void OnUpdate(f32 Delta){}
        virtual void OnPhysicsUpdate(f32 Delta){}
        virtual void OnExit(){}
        
        void SetStateMachine(StateMachine* InStateMachine)
        {
            m_StateMachine = InStateMachine;
        }
        
        template<typename StateMachineType> requires IsBaseOfValue<StateMachine, StateMachineType>
        StateMachineType* GetStateMachine() { return dynamic_cast<StateMachineType*>(m_StateMachine); }
    private:
        StateMachine* m_StateMachine = nullptr;
    };
}
