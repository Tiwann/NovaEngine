#pragma once
#include "PhysicsComponent.h"
#include "Physics/PhysicsContactInfo3D.h"

namespace Nova
{
    struct Contact3D;
    struct PhysicsMaterial;
    class PhysicsBody3D;
    class PhysicsShape3D;
    
    class RigidBody3D : public PhysicsComponent, public IPhysics<PhysicsBody3D, PhysicsShape3D, Contact3D>
    {
    public:
        friend class Entity;
        RigidBody3D(Entity* Owner, const String& Name);
        
        void OnInit() override;
        void OnStart() override;
        void OnDestroy() override;
        void OnPhysicsUpdate(f32 Delta) override;
        void OnInspectorGUI(const ImGuiIO& IO) override;

        void OnContactBegin(const PhysicsContactInfo3D& ContactInfo);
        void OnContactStay(const PhysicsContactInfo3D& ContactInfo);
        void OnContactEnd(const PhysicsContactInfo3D& ContactInfo);
        
        void SetGravityScale(f32 Scale) override;
        void SetLinearVelocity(const Vector3& Velocity) override;
        void SetAngularVelocity(const Vector3& AngularVelocity) override;
        void SetLinearDamping(f32 LinearDamping) override;
        void SetAngularDamping(f32 AngularDamping) override;
        f32 GetGravityScale() const override;
        Vector3 GetLinearVelocity() const override;
        Vector3 GetAngularVelocity() const override;
        f32 GetLinearDamping() const override;
        f32 GetAngularDamping() const override;
        Vector3 GetLinearVelocityPoint(const Vector3& Point) const override;
        void AddForce(const Vector3& Force) override;
        void AddImpulse(const Vector3& Force) override;
        void AddForceAtPosition(const Vector3& Position, const Vector3& Force) override;
        void AddImpulseAtPosition(const Vector3& Position, const Vector3& Force) override;
        void RecreatePhysicsState() override;
        
        const PhysicsMaterial& GetMaterial() const override;
        void SetMaterial(const PhysicsMaterial& Material) override;
        PhysicsConstraintsFlags GetConstraints() override;
        void SetConstraints(PhysicsConstraintsFlags Constraints) override;
        bool IsSensor() const override;
        void SetSensor(bool Sensor) override;
        PhysicsBodyType GetPhysicsBodyType() const override;
        void SetPhysicsBodyType(PhysicsBodyType Type) override;
        void SetPosition(const Vector3& Position) override;
        void SetRotation(const Quaternion& Rotation) override;
    };
}
