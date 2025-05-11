#include "PhysicsBody2D.h"
#include "PhysicsMaterial.h"
#include "PhysicsWorld2D.h"
#include "PhysicsShape2D.h"
#include "Math/Functions.h"
#include "Math/Vector2.h"
#include "../Math/MathConversions.h"
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>

namespace Nova
{
    PhysicsBody2D::PhysicsBody2D(b2Body* Handle, PhysicsWorld2D& World) : PhysicsBody(Handle, World)
    {
        
    }

    void PhysicsBody2D::SetUserPointer(void* User)
    {
        b2Body* BodyHandle = GetHandle();
        b2BodyUserData& UserData = BodyHandle->GetUserData();
        UserData.pointer = (uintptr_t)User;
    }

    void* PhysicsBody2D::GetUserPointer() const
    {
        const b2Body* BodyHandle = GetHandle();
        const b2BodyUserData& UserData = BodyHandle->GetUserData();
        return (void*)UserData.pointer;
    }

    void PhysicsBody2D::CreatePhysicsState(PhysicsShape2D* Shape, const PhysicsMaterial& Material)
    {
        b2FixtureDef FixtureDefinition = {};
        const b2Shape& ShapeHandle = Shape->GetHandle();
        
        FixtureDefinition.shape = &ShapeHandle;
        FixtureDefinition.density = Material.Density;
        FixtureDefinition.friction = Material.Friction;
        FixtureDefinition.restitution = Material.Bounciness;
        FixtureDefinition.isSensor = m_IsSensor;
        
        b2FixtureUserData FixtureUserData;
        FixtureUserData.pointer = (uintptr_t)this;
        FixtureDefinition.userData = FixtureUserData;
        b2Body* BodyHandle = GetHandle();
        m_Fixture = BodyHandle->CreateFixture(&FixtureDefinition);
    }

    void PhysicsBody2D::DestroyPhysicsState()
    {
        b2Body* BodyHandle = GetHandle();
        BodyHandle->DestroyFixture(m_Fixture);
        m_Fixture = nullptr;
    }
    
    void PhysicsBody2D::SetPosition(const Vector3& Position)
    {
        b2Body* BodyHandle = GetHandle();
        const f32 Angle = BodyHandle->GetAngle();
        BodyHandle->SetTransform(ToB2Vec2(Vector2(Position)), Angle);
    }

    Vector3 PhysicsBody2D::GetPosition() const
    {
        const b2Body* BodyHandle = GetHandle();
        return Vector3(ToVector2(BodyHandle->GetPosition()));
    }

    void PhysicsBody2D::SetRotation(const Vector3& Rotation)
    {
        b2Body* BodyHandle = GetHandle();
        BodyHandle->SetTransform(BodyHandle->GetPosition(), Math::Radians(Rotation.z));
    }

    Vector3 PhysicsBody2D::GetRotation() const
    {
        const b2Body* BodyHandle = GetHandle();
        return {0.0f, 0.0, Math::Degrees(BodyHandle->GetAngle())};
    }

    void PhysicsBody2D::SetPositionAndRotation(const Vector3& Position, const Vector3& Rotation)
    {
        b2Body* BodyHandle = GetHandle();
        BodyHandle->SetTransform(ToB2Vec2(Vector2(Position)), Math::Radians(Rotation.z));
    }

    void PhysicsBody2D::SetGravityScale(f32 Scale)
    {
        b2Body* BodyHandle = GetHandle();
        BodyHandle->SetGravityScale(Scale);
    }

    void PhysicsBody2D::SetLinearVelocity(const Vector3& Velocity)
    {
        b2Body* BodyHandle = GetHandle();
        BodyHandle->SetLinearVelocity(ToB2Vec2(Vector2(Velocity)));
    }

    void PhysicsBody2D::SetAngularVelocity(const Vector3& AngularVelocity)
    {
        b2Body* BodyHandle = GetHandle();
        BodyHandle->SetAngularVelocity(AngularVelocity.z);
    }

    void PhysicsBody2D::SetLinearDamping(f32 LinearDamping)
    {
        b2Body* BodyHandle = GetHandle();
        BodyHandle->SetLinearDamping(LinearDamping);
    }

    void PhysicsBody2D::SetAngularDamping(f32 AngularDamping)
    {
        b2Body* BodyHandle = GetHandle();
        BodyHandle->SetAngularDamping(AngularDamping);
    }

    f32 PhysicsBody2D::GetGravityScale() const
    {
        const b2Body* BodyHandle = GetHandle();
        return BodyHandle->GetGravityScale();
    }

    Vector3 PhysicsBody2D::GetLinearVelocity() const
    {
        const b2Body* BodyHandle = GetHandle();
        return Vector3(ToVector2(BodyHandle->GetLinearVelocity()));
    }

    Vector3 PhysicsBody2D::GetLinearVelocityPoint(const Vector3& Point) const
    {
        const b2Body* BodyHandle = GetHandle();
        return Vector3(ToVector2(BodyHandle->GetLinearVelocityFromLocalPoint(ToB2Vec2(Vector2(Point)))));
    }

    Vector3 PhysicsBody2D::GetAngularVelocity() const
    {
        const b2Body* BodyHandle = GetHandle();
        return {0.0f, 0.0f, BodyHandle->GetAngularVelocity()};
    }

    f32 PhysicsBody2D::GetLinearDamping() const
    {
        const b2Body* BodyHandle = GetHandle();
        return BodyHandle->GetLinearDamping();
    }

    f32 PhysicsBody2D::GetAngularDamping() const
    {
        const b2Body* BodyHandle = GetHandle();
        return BodyHandle->GetAngularDamping();
    }

    void PhysicsBody2D::AddForce(const Vector3& Force)
    {
        b2Body* BodyHandle = GetHandle();
        BodyHandle->ApplyForce(ToB2Vec2(Vector2(Force)), { 0.0f, 0.0f }, true);
    }

    void PhysicsBody2D::AddImpulse(const Vector3& Force)
    {
        b2Body* BodyHandle = GetHandle();
        BodyHandle->ApplyLinearImpulse(ToB2Vec2(Vector2(Force)), { 0.0f, 0.0f }, true);
    }

    void PhysicsBody2D::AddForceAtPosition(const Vector3& Position, const Vector3& Force)
    {
        b2Body* BodyHandle = GetHandle();
        BodyHandle->ApplyForce(ToB2Vec2(Vector2(Force)), ToB2Vec2(Vector2(Position)), true);
    }

    void PhysicsBody2D::AddImpulseAtPosition(const Vector3& Position, const Vector3& Force)
    {
        b2Body* BodyHandle = GetHandle();
        BodyHandle->ApplyLinearImpulse(ToB2Vec2(Vector2(Force)), ToB2Vec2(Vector2(Position)), true);
    }

    const PhysicsConstraintsFlags& PhysicsBody2D::GetConstraints() const
    {
        return m_Constraints;
    }

    void PhysicsBody2D::SetConstraints(const PhysicsConstraintsFlags& Constraints)
    {
        m_Constraints = Constraints;
        const bool FixedRotation = m_Constraints.Contains(PhysicsConstraintsBits::Rotation) ||
                m_Constraints.Contains(PhysicsConstraintsBits::RotationZ);
        b2Body* BodyHandle = GetHandle();
        BodyHandle->SetFixedRotation(FixedRotation);
    }

    const PhysicsMaterial& PhysicsBody2D::GetMaterial() const
    {
        return m_Material;
    }

    void PhysicsBody2D::SetMaterial(const PhysicsMaterial& Material)
    {
        m_Material = Material;
        m_Fixture->SetFriction(Material.Friction);
        m_Fixture->SetRestitution(Material.Bounciness);
        m_Fixture->SetDensity(Material.Density);
    }

    PhysicsBodyType PhysicsBody2D::GetType() const
    {
        return m_Type;
    }

    void PhysicsBody2D::SetType(PhysicsBodyType Type)
    {
        m_Type = Type;
        b2Body* BodyHandle = GetHandle();
        BodyHandle->SetType((b2BodyType)Type);
    }

    bool PhysicsBody2D::IsSensor()
    {
        return m_IsSensor;
    }

    void PhysicsBody2D::SetIsSensor(bool Sensor)
    {
        m_IsSensor = Sensor;
        m_Fixture->SetSensor(Sensor);
    }
}
