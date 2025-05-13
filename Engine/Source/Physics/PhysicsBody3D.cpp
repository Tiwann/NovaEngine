#include "PhysicsBody3D.h"
#include "PhysicsWorld3D.h"
#include "PhysicsShape3D.h"
#include "Math/Functions.h"
#include "PhysicsMaterial.h"
#include "Math/Quaternion.h"
#include "Math/MathConversions.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>


namespace Nova
{
    void PhysicsBody3D::SetUserPointer(void* User)
    {
        JPH::Body* BodyHandle = GetHandle();
        BodyHandle->SetUserData((JPH::uint64)User);
    }

    void* PhysicsBody3D::GetUserPointer() const
    {
        const JPH::Body* BodyHandle = GetHandle();
        return (void*)BodyHandle->GetUserData();
    }

    void PhysicsBody3D::CreatePhysicsState(PhysicsShape3D* Shape, const PhysicsMaterial& Material)
    {
        const PhysicsWorld3D* World = GetWorld();
        const JPH::PhysicsSystem& System = World->GetSystem();
        const JPH::BodyInterface& BodyInterface = System.GetBodyInterface();
        const JPH::Body* BodyHandle = GetHandle();
        const JPH::Shape& ShapeHandle = Shape->GetHandle();
        BodyInterface.SetShape(BodyHandle->GetID(), &ShapeHandle, true, JPH::EActivation::Activate);
    }

    void PhysicsBody3D::DestroyPhysicsState()
    {
        
    }

    void PhysicsBody3D::SetPosition(const Vector3& Position)
    {
        PhysicsWorld3D* World = GetWorld();
        JPH::PhysicsSystem& System = World->GetSystem();
        JPH::BodyInterface& BodyInterface = System.GetBodyInterface();
        const JPH::Body* BodyHandle = GetHandle();
        BodyInterface.SetPosition(BodyHandle->GetID(), ToJoltVec3(Position), JPH::EActivation::Activate);
    }

    Vector3 PhysicsBody3D::GetPosition() const
    {
        const PhysicsWorld3D* World = GetWorld();
        const JPH::PhysicsSystem& System = World->GetSystem();
        const JPH::BodyInterface& BodyInterface = System.GetBodyInterface();
        const JPH::Body* BodyHandle = GetHandle();
        const JPH::Vec3 Result = BodyInterface.GetPosition(BodyHandle->GetID());
        return ToVector3(Result);
    }

    void PhysicsBody3D::SetRotation(const Quaternion& Rotation)
    {
        PhysicsWorld3D* World = GetWorld();
        JPH::PhysicsSystem& System = World->GetSystem();
        JPH::BodyInterface& BodyInterface = System.GetBodyInterface();
        const JPH::Body* BodyHandle = GetHandle();
        const JPH::Quat NewRotation = JPH::Quat(Rotation.x, Rotation.y, Rotation.z, Rotation.w);
        BodyInterface.SetRotation(BodyHandle->GetID(), NewRotation, JPH::EActivation::Activate);
    }

    Vector3 PhysicsBody3D::GetRotation() const
    {
        const PhysicsWorld3D* World = GetWorld();
        const JPH::PhysicsSystem& System = World->GetSystem();
        const JPH::BodyInterface& BodyInterface = System.GetBodyInterface();
        const JPH::Body* BodyHandle = GetHandle();
        return ToVector3(BodyInterface.GetRotation(BodyHandle->GetID()).GetEulerAngles()).Apply(Math::Degrees);
    }

    void PhysicsBody3D::SetPositionAndRotation(const Vector3& Position, const Vector3& Rotation)
    {
        PhysicsWorld3D* World = GetWorld();
        JPH::PhysicsSystem& System = World->GetSystem();
        JPH::BodyInterface& BodyInterface = System.GetBodyInterface();
        const JPH::Body* BodyHandle = GetHandle();
        const JPH::Quat NewRotation = JPH::Quat::sEulerAngles(ToJoltVec3(Rotation.Apply(Math::Radians)));
        BodyInterface.SetPositionAndRotation(BodyHandle->GetID(), ToJoltVec3(Position), NewRotation, JPH::EActivation::Activate);
    }

    void PhysicsBody3D::SetGravityScale(f32 Scale)
    {
        JPH::Body* BodyHandle = GetHandle();
        JPH::MotionProperties* MotionProperties = BodyHandle->GetMotionProperties();
        MotionProperties->SetGravityFactor(Scale);
    }

    void PhysicsBody3D::SetLinearVelocity(const Vector3& Velocity)
    {
        JPH::Body* BodyHandle = GetHandle();
        JPH::MotionProperties* MotionProperties = BodyHandle->GetMotionProperties();
        MotionProperties->SetLinearVelocity(ToJoltVec3(Velocity));
    }

    void PhysicsBody3D::SetAngularVelocity(const Vector3& AngularVelocity)
    {
        JPH::Body* BodyHandle = GetHandle();
        JPH::MotionProperties* MotionProperties = BodyHandle->GetMotionProperties();
        MotionProperties->SetAngularVelocity(ToJoltVec3(AngularVelocity));
    }

    void PhysicsBody3D::SetLinearDamping(f32 LinearDamping)
    {
        JPH::Body* BodyHandle = GetHandle();
        JPH::MotionProperties* MotionProperties = BodyHandle->GetMotionProperties();
        MotionProperties->SetLinearDamping(LinearDamping);
    }

    void PhysicsBody3D::SetAngularDamping(f32 AngularDamping)
    {
        JPH::Body* BodyHandle = GetHandle();
        JPH::MotionProperties* MotionProperties = BodyHandle->GetMotionProperties();
        MotionProperties->SetAngularDamping(AngularDamping);
    }

    f32 PhysicsBody3D::GetGravityScale() const
    {
        const JPH::Body* BodyHandle = GetHandle();
        const JPH::MotionProperties* MotionProperties = BodyHandle->GetMotionProperties();
        return MotionProperties->GetGravityFactor();
    }

    Vector3 PhysicsBody3D::GetLinearVelocity() const
    {
        const JPH::Body* BodyHandle = GetHandle();
        const JPH::MotionProperties* MotionProperties = BodyHandle->GetMotionProperties();
        return ToVector3(MotionProperties->GetLinearVelocity());
    }


    Vector3 PhysicsBody3D::GetLinearVelocityPoint(const Vector3& Point) const
    {
        const JPH::Body* BodyHandle = GetHandle();
        const JPH::MotionProperties* MotionProperties = BodyHandle->GetMotionProperties();
        return ToVector3(MotionProperties->GetPointVelocityCOM(ToJoltVec3(Point)));
    }


    Vector3 PhysicsBody3D::GetAngularVelocity() const
    {
        const JPH::Body* BodyHandle = GetHandle();
        const JPH::MotionProperties* MotionProperties = BodyHandle->GetMotionProperties();
        return ToVector3(MotionProperties->GetAngularVelocity());
    }

    f32 PhysicsBody3D::GetLinearDamping() const
    {
        const JPH::Body* BodyHandle = GetHandle();
        const JPH::MotionProperties* MotionProperties = BodyHandle->GetMotionProperties();
        return MotionProperties->GetLinearDamping();
    }

    f32 PhysicsBody3D::GetAngularDamping() const
    {
        const JPH::Body* BodyHandle = GetHandle();
        const JPH::MotionProperties* MotionProperties = BodyHandle->GetMotionProperties();
        return MotionProperties->GetAngularDamping();
    }

    void PhysicsBody3D::AddForce(const Vector3& Force)
    {
        JPH::Body* BodyHandle = GetHandle();
        BodyHandle->AddForce(ToJoltVec3(Force));
    }

    void PhysicsBody3D::AddImpulse(const Vector3& Force)
    {
        JPH::Body* BodyHandle = GetHandle();
        BodyHandle->AddImpulse(ToJoltVec3(Force));
    }

    void PhysicsBody3D::AddForceAtPosition(const Vector3& Position, const Vector3& Force)
    {
        JPH::Body* BodyHandle = GetHandle();
        BodyHandle->AddForce(ToJoltVec3(Force), ToJoltVec3(Position));
    }

    void PhysicsBody3D::AddImpulseAtPosition(const Vector3& Position, const Vector3& Force)
    {
        JPH::Body* BodyHandle = GetHandle();
        BodyHandle->AddImpulse(ToJoltVec3(Force), ToJoltVec3(Position));
    }

    const PhysicsConstraintsFlags& PhysicsBody3D::GetConstraints() const
    {
        return m_Constraints;
    }

    void PhysicsBody3D::SetConstraints(const PhysicsConstraintsFlags& Constraints)
    {
        m_Constraints = Constraints;
        // TODO: Jolt Physics constrains handling (JPH::EAllowedDOFs)
    }

    const PhysicsMaterial& PhysicsBody3D::GetMaterial() const
    {
        return m_Material;
    }

    void PhysicsBody3D::SetMaterial(const PhysicsMaterial& Material)
    {
        m_Material = Material;
        JPH::Body* BodyHandle = GetHandle();
        BodyHandle->SetFriction(m_Material.Friction);
        BodyHandle->SetRestitution(m_Material.Bounciness);
    }

    PhysicsBodyType PhysicsBody3D::GetType() const
    {
        return m_Type;
    }

    void PhysicsBody3D::SetType(PhysicsBodyType Type)
    {
        m_Type = Type;
        PhysicsWorld3D* World = GetWorld();
        JPH::PhysicsSystem& System = World->GetSystem();
        JPH::BodyInterface& Interface = System.GetBodyInterface();
        const JPH::Body* BodyHandle = GetHandle();
        Interface.SetMotionType(BodyHandle->GetID(), (JPH::EMotionType)m_Type, JPH::EActivation::Activate);
    }

    bool PhysicsBody3D::IsSensor()
    {
        return m_IsSensor;
    }

    void PhysicsBody3D::SetIsSensor(bool Sensor)
    {
        m_IsSensor = Sensor;
        JPH::Body* BodyHandle = GetHandle();
        BodyHandle->SetIsSensor(m_IsSensor);
    }
}
