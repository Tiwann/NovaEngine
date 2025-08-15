#include "PhysicsComponent.h"
#include "Components/Transform.h"
#include "Physics/PhysicsBody.h"
#include "Physics/PhysicsShape.h"

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

    void PhysicsComponent::SetBodyPosition(const Vector3& position)
    {
        m_Body->SetPosition(position);
    }

    Vector3 PhysicsComponent::GetBodyPosition() const
    {
        return m_Body->GetPosition();
    }

    void PhysicsComponent::SetBodyRotation(const Quaternion& rotation)
    {
        m_Body->SetRotation(rotation);
    }

    Quaternion PhysicsComponent::GetBodyRotation() const
    {
        return m_Body->GetRotation();
    }

    void PhysicsComponent::SetPositionAndRotation(const Vector3& position, const Quaternion& rotation)
    {
        m_Body->SetPosition(position);
    }

    void PhysicsComponent::SetLinearVelocity(const Vector3& velocity)
    {
        m_Body->SetLinearVelocity(velocity);
    }

    Vector3 PhysicsComponent::GetLinearVelocity() const
    {
        return m_Body->GetLinearVelocity();
    }

    void PhysicsComponent::SetAngularVelocity(const Vector3& angularVelocity)
    {
        m_Body->SetAngularVelocity(angularVelocity);
    }

    Vector3 PhysicsComponent::GetAngularVelocity() const
    {
        return m_Body->GetAngularVelocity();
    }

    void PhysicsComponent::SetLinearDamping(const float linearDamping)
    {
        m_Body->SetLinearDamping(linearDamping);
    }

    float PhysicsComponent::GetLinearDamping() const
    {
        return m_Body->GetLinearDamping();
    }

    void PhysicsComponent::SetAngularDamping(const float angularDamping)
    {
        m_Body->SetAngularDamping(angularDamping);
    }

    float PhysicsComponent::GetAngularDamping() const
    {
        return m_Body->GetAngularDamping();
    }

    Vector3 PhysicsComponent::GetLinearVelocityPoint(const Vector3& localPoint) const
    {
        return m_Body->GetLinearVelocityPoint(localPoint);
    }

    void PhysicsComponent::AddForce(const Vector3& force)
    {
        m_Body->AddForce(force);
    }

    void PhysicsComponent::AddImpulse(const Vector3& impulse)
    {
        m_Body->AddImpulse(impulse);
    }

    void PhysicsComponent::AddForceAtPosition(const Vector3& position, const Vector3& force)
    {
        m_Body->AddForceAtPosition(position, force);
    }

    void PhysicsComponent::AddImpulseAtPosition(const Vector3& position, const Vector3& impulse)
    {
        m_Body->AddImpulseAtPosition(position, impulse);
    }

    const PhysicsConstraintsFlags& PhysicsComponent::GetConstraints() const
    {
        return m_Body->GetConstraints();
    }

    void PhysicsComponent::SetConstraints(const PhysicsConstraintsFlags& constraints)
    {
        m_Body->SetConstraints(constraints);
    }

    PhysicsBodyType PhysicsComponent::GetType() const
    {
        return m_Body->GetType();
    }

    void PhysicsComponent::SetType(const PhysicsBodyType type)
    {
        m_Body->SetType(type);
    }

    void PhysicsComponent::SetMaterial(const Ref<PhysicsMaterial>& material)
    {
        m_Shape->SetPhysicsMaterial(material);
    }

    Ref<PhysicsMaterial> PhysicsComponent::GetMaterial() const
    {
        return m_Shape->GetPhysicsMaterial();
    }

    void PhysicsComponent::SetGravityScale(const float scale)
    {
        m_Body->SetGravityScale(scale);
    }

    float PhysicsComponent::GetGravityScale() const
    {
        return m_Body->GetGravityScale();
    }
}
