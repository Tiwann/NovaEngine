#pragma once
#include "Physics/PhysicsBodyType.h"
#include "Physics/PhysicsConstraints.h"
#include "Runtime/Component.h"
#include "Runtime/Ref.h"

namespace Nova
{
    class PhysicsMaterial;
    class PhysicsBody;
    class PhysicsShape;

    class PhysicsComponent : public Component
    {
    public:
        PhysicsComponent(Entity* owner, const String& name) : Component(owner, name) {}

        void OnPhysicsUpdate(float deltaTime) override;
        PhysicsBody* GetPhysicsBody();
        const PhysicsBody* GetPhysicsBody() const;

        void SetBodyPosition(const Vector3& position);
        Vector3 GetBodyPosition() const;
        void SetBodyRotation(const Quaternion& rotation);
        Quaternion GetBodyRotation() const;
        void SetBodyPositionAndRotation(const Vector3& position, const Quaternion& rotation);

        void SetLinearVelocity(const Vector3& velocity);
        Vector3 GetLinearVelocity() const;
        void SetAngularVelocity(const Vector3& angularVelocity);
        Vector3 GetAngularVelocity() const;

        void SetLinearDamping(float linearDamping);
        float GetLinearDamping() const;
        void SetAngularDamping(float angularDamping);
        float GetAngularDamping() const;

        Vector3 GetLinearVelocityPoint(const Vector3& localPoint) const;
        void AddForce(const Vector3& force);
        void AddImpulse(const Vector3& impulse);
        void AddForceAtPosition(const Vector3& position, const Vector3& force);
        void AddImpulseAtPosition(const Vector3& position, const Vector3& impulse);

        const PhysicsConstraintsFlags& GetConstraints() const;
        void SetConstraints(const PhysicsConstraintsFlags& constraints);

        PhysicsBodyType GetType() const;
        void SetType(PhysicsBodyType type);

        void SetMaterial(const Ref<PhysicsMaterial>& material);
        Ref<PhysicsMaterial> GetMaterial() const;

        void SetGravityScale(float scale);
        float GetGravityScale() const;
    protected:
        PhysicsBody* m_Body = nullptr;
        PhysicsShape* m_Shape = nullptr;
    };
}
