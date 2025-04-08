#include "ScriptComponent.h"
#include "Components/Physics/RigidBody2D.h"
#include "Components/Physics/RigidBody3D.h"
#include "Runtime/Entity.h"
#include "Containers/Array.h"

namespace Nova
{
    void ScriptComponent::OnInit()
    {
        Component::OnInit();

        for(const Array<RigidBody2D*> RB2Ds = m_Entity->GetAllComponents<RigidBody2D>(); RigidBody2D* Rigidbody : RB2Ds)
        {
            Rigidbody->OnContactBeginEvent.BindMember(this, &ScriptComponent::OnContactBegin2D);
            Rigidbody->OnContactStayEvent.BindMember(this, &ScriptComponent::OnContactStay2D);
            Rigidbody->OnContactEndEvent.BindMember(this, &ScriptComponent::OnContactEnd2D);

            Rigidbody->OnTriggerBeginEvent.BindMember(this, &ScriptComponent::OnTriggerBegin2D);
            Rigidbody->OnTriggerStayEvent.BindMember(this, &ScriptComponent::OnTriggerStay2D);
            Rigidbody->OnTriggerEndEvent.BindMember(this, &ScriptComponent::OnTriggerEnd2D);
        }

        for(const Array<RigidBody3D*> RB3Ds = m_Entity->GetAllComponents<RigidBody3D>(); RigidBody3D* Rigidbody : RB3Ds)
        {
            Rigidbody->OnContactBeginEvent.BindMember(this, &ScriptComponent::OnContactBegin3D);
            Rigidbody->OnContactStayEvent.BindMember(this, &ScriptComponent::OnContactStay3D);
            Rigidbody->OnContactEndEvent.BindMember(this, &ScriptComponent::OnContactEnd3D);

            Rigidbody->OnTriggerBeginEvent.BindMember(this, &ScriptComponent::OnTriggerBegin3D);
            Rigidbody->OnTriggerStayEvent.BindMember(this, &ScriptComponent::OnTriggerStay3D);
            Rigidbody->OnTriggerEndEvent.BindMember(this, &ScriptComponent::OnTriggerEnd3D);
        }
    }

    void ScriptComponent::OnDestroy()
    {
        Component::OnDestroy();
    }
}
