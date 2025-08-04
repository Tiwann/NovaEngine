#pragma once
#include "PhysicsBody.h"
#include <box2d/id.h>

namespace Nova
{
    class PhysicsWorld2D;
    struct PhysicsContactInfo2D;
    
    class PhysicsBody2D : public PhysicsBody
    {
    public:
        explicit PhysicsBody2D(b2BodyId handle, const PhysicsWorld2D& world);

        void SetPosition(const Vector3& position) override;
        Vector3 GetPosition() const override;
        void SetRotation(const Quaternion& rotation) override;
        Quaternion GetRotation() const override;
        void SetPositionAndRotation(const Vector3& position, const Quaternion& rotation) override;

        void SetLinearVelocity(const Vector3& velocity) override;
        Vector3 GetLinearVelocity() const override;
        void SetAngularVelocity(const Vector3& angularVelocity) override;
        Vector3 GetAngularVelocity() const override;

        void SetLinearDamping(float linearDamping) override;
        float GetLinearDamping() const override;
        void SetAngularDamping(float angularDamping) override;
        float GetAngularDamping() const override;

        Vector3 GetLinearVelocityPoint(const Vector3& localPoint) const override;
        void AddForce(const Vector3& force) override;
        void AddImpulse(const Vector3& impulse) override;
        void AddForceAtPosition(const Vector3& position, const Vector3& force) override;
        void AddImpulseAtPosition(const Vector3& position, const Vector3& impulse) override;

        const PhysicsConstraintsFlags& GetConstraints() const override;
        void SetConstraints(const PhysicsConstraintsFlags& constraints) override;

        const PhysicsMaterial& GetMaterial() const override;
        void SetMaterial(const PhysicsMaterial& material) override;

        PhysicsBodyType GetType() const override;
        void SetType(PhysicsBodyType type) override;

        bool IsSensor() override;
        void SetIsSensor(bool isSensor) override;

        b2BodyId GetHandle() const;

    private:
        b2BodyId m_Handle = b2_nullBodyId;
    };
}
