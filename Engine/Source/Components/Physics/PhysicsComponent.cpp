#include "PhysicsComponent.h"
#include "Components/Transform.h"
#include "Physics/PhysicsWorld.h"

namespace Nova
{
    void PhysicsComponent::OnPhysicsUpdate(const float deltaTime)
    {
        Component::OnPhysicsUpdate(deltaTime);
        const Vector3 position = m_Body->GetPosition();
        const Quaternion rotation = m_Body->GetRotation();

        Transform* transform = GetTransform();
        transform->SetPosition(position);
        transform->SetRotation(rotation);
    }

    PhysicsBody* PhysicsComponent::GetPhysicsBody()
    {
        return m_Body;
    }

    const PhysicsBody* PhysicsComponent::GetPhysicsBody() const
    {
        return m_Body;
    }
}
