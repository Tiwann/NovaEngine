#pragma once
#include "PhysicsBody.h"
#include <CoreExport.h>

class b2Body;
class b2Fixture;

namespace Nova
{
    class PhysicsWorld2D;
    struct PhysicsContactInfo2D;
    class PhysicsShape2D;
    
    class CORE_API PhysicsBody2D final : public PhysicsBody<PhysicsShape2D, PhysicsWorld2D, b2Body, PhysicsContactInfo2D>
    {
    public:
        PhysicsBody2D(b2Body* Handle, PhysicsWorld2D& World);

        void SetUserPointer(void* User) override;
        void* GetUserPointer() const override;
    
        void CreatePhysicsState(PhysicsShape2D* Shape, const PhysicsMaterial& Material) override;
        void DestroyPhysicsState() override;
        
        
        void SetPosition(const Vector3& Position) override;
        Vector3 GetPosition() const override;
        void SetRotation(const Vector3& Rotation) override;
        Vector3 GetRotation() const override;
        void SetPositionAndRotation(const Vector3& Position, const Vector3& Rotation) override;

        void SetGravityScale(f32 Scale) override;
        void SetLinearVelocity(const Vector3& Velocity) override;
        void SetAngularVelocity(const Vector3& AngularVelocity) override;
        void SetLinearDamping(f32 LinearDamping) override;
        void SetAngularDamping(f32 AngularDamping) override;
        
        f32 GetGravityScale() const override;
        Vector3 GetLinearVelocity() const override;
        Vector3 GetLinearVelocityPoint(const Vector3& Point) const override;
        Vector3 GetAngularVelocity() const override;
        f32 GetLinearDamping() const override;
        f32 GetAngularDamping() const override;

        void AddForce(const Vector3& Force) override;
        void AddImpulse(const Vector3& Force) override;
        void AddForceAtPosition(const Vector3& Position, const Vector3& Force) override;
        void AddImpulseAtPosition(const Vector3& Position, const Vector3& Force) override;

        const PhysicsConstraintsFlags& GetConstraints() const override;
        void SetConstraints(const PhysicsConstraintsFlags& Constraints) override;
        void SetMaterial(const PhysicsMaterial& Material) override;
        const PhysicsMaterial& GetMaterial() const override;
        PhysicsBodyType GetType() const override;
        void SetType(PhysicsBodyType Type) override;
        bool IsSensor() override;
        void SetIsSensor(bool Sensor) override;
    private:
        b2Fixture* m_Fixture = nullptr;
    };
}
