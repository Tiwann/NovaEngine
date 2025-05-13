#pragma once
#include "PhysicsComponent.h"

namespace Nova
{
    struct Vector3;
    class Transform;
    struct PhysicsMaterial;
    struct Contact2D;
    class PhysicsBody2D;
    class PhysicsShape2D;
    struct PhysicsContactInfo2D;
    
    class RigidBody2D : public PhysicsComponent, public IPhysics<PhysicsBody2D, PhysicsShape2D, Contact2D>
    {
    protected:
        friend class Entity;
        RigidBody2D(Entity* Owner, const String& Name);
        
        void OnInit() override;
        void OnStart() override;
        void OnDestroy() override;
        void OnInspectorGUI(const ImGuiIO& IO) override;
        void OnPhysicsUpdate(f32 Delta) override;

        void OnContactBegin(const PhysicsContactInfo2D& ContactInfo);
        void OnContactStay(const PhysicsContactInfo2D& ContactInfo);
        void OnContactEnd(const PhysicsContactInfo2D& ContactInfo);
    public:
        f32 GetGravityScale() const override;
        void SetGravityScale(f32 Scale) override;
        void SetLinearVelocity(const Vector3& Velocity) override;
        void SetAngularVelocity(const Vector3& AngularVelocity) override;
        void SetLinearDamping(f32 LinearDamping) override;
        void SetAngularDamping(f32 AngularDamping) override;
        
        Vector3 GetLinearVelocity() const override;
        Vector3 GetAngularVelocity() const override;
        f32 GetLinearDamping() const override;
        f32 GetAngularDamping() const override;
        Vector3 GetLinearVelocityPoint(const Vector3& Point) const override;
        
        void AddForce(const Vector3& Force) override;
        void AddImpulse(const Vector3& Force) override;
        void AddForceAtPosition(const Vector3& Position, const Vector3& Force) override;
        void AddImpulseAtPosition(const Vector3& Position, const Vector3& Force) override;
        void SetPosition(const Vector3& Position) override;
        void SetRotation(const Quaternion& Rotation) override;
        void RecreatePhysicsState() override;
        
        const PhysicsMaterial& GetMaterial() const override;
        void SetMaterial(const PhysicsMaterial& Material) override;
        PhysicsConstraintsFlags GetConstraints() override;
        void SetConstraints(PhysicsConstraintsFlags Constraints) override;
        bool IsSensor() const override;
        void SetSensor(bool Sensor) override;
        PhysicsBodyType GetPhysicsBodyType() const override;
        void SetPhysicsBodyType(PhysicsBodyType Type) override;
    };
}
