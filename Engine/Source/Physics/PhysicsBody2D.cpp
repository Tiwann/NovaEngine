#include "PhysicsBody2D.h"
#include "PhysicsMaterial.h"
#include "PhysicsWorld2D.h"
#include "Math/Quaternion.h"
#include "Box2DHelpers.h"

#include <box2d/box2d.h>

namespace Nova
{
    PhysicsBody2D::PhysicsBody2D(const b2BodyId handle, const PhysicsWorld2D& world) : PhysicsBody(world), m_Handle(handle)
    {

    }

    void PhysicsBody2D::SetPosition(const Vector3& position)
    {
        const b2Transform transform = b2Body_GetTransform(m_Handle);
        b2Body_SetTransform(m_Handle, b2Vec2(position.x, position.y), transform.q);
    }

    Vector3 PhysicsBody2D::GetPosition() const
    {
        const b2Transform transform = b2Body_GetTransform(m_Handle);
        return Vector3(transform.p.x, transform.p.y, 0.0f);
    }

    void PhysicsBody2D::SetRotation(const Quaternion& rotation)
    {
        const b2Transform transform = b2Body_GetTransform(m_Handle);
        const float radians = rotation.ToEuler().z;
        b2Body_SetTransform(m_Handle, transform.p, b2MakeRot(radians));
    }

    Quaternion PhysicsBody2D::GetRotation() const
    {
        const b2Transform transform = b2Body_GetTransform(m_Handle);
        const b2Vec2 axisX = b2Rot_GetXAxis(transform.q);
        const b2Vec2 axisY = b2Rot_GetYAxis(transform.q);
        const Vector3 axis = Vector3(axisX.x, axisY.y, 0.0f).Cross(Vector3(axisY.x, axisX.y, 0.0f));
        return Quaternion::FromAxisAngle(axis, b2Rot_GetAngle(transform.q));
    }

    void PhysicsBody2D::SetPositionAndRotation(const Vector3& position, const Quaternion& rotation)
    {
        const float radians = rotation.ToEuler().z;

        b2Body_SetTransform(m_Handle, b2Vec2(position.x, position.y), b2MakeRot(radians));
    }

    void PhysicsBody2D::SetLinearVelocity(const Vector3& velocity)
    {
        b2Body_SetLinearVelocity(m_Handle, b2Vec2(velocity.x, velocity.y));
    }

    Vector3 PhysicsBody2D::GetLinearVelocity() const
    {
        const b2Vec2 linearVelocity = b2Body_GetLinearVelocity(m_Handle);
        return Vector3(linearVelocity.x, linearVelocity.y, 0.0f);
    }

    void PhysicsBody2D::SetAngularVelocity(const Vector3& angularVelocity)
    {
        b2Body_SetAngularVelocity(m_Handle, angularVelocity.z);
    }

    Vector3 PhysicsBody2D::GetAngularVelocity() const
    {
        const float angularVelocity = b2Body_GetAngularVelocity(m_Handle);
        return Vector3(0.0f, 0.0f, angularVelocity);
    }

    void PhysicsBody2D::SetLinearDamping(const float linearDamping)
    {
        b2Body_SetLinearDamping(m_Handle, linearDamping);
    }

    float PhysicsBody2D::GetLinearDamping() const
    {
        return b2Body_GetLinearDamping(m_Handle);
    }

    void PhysicsBody2D::SetAngularDamping(const float angularDamping)
    {
        b2Body_SetAngularDamping(m_Handle, angularDamping);
    }

    float PhysicsBody2D::GetAngularDamping() const
    {
        return b2Body_GetAngularDamping(m_Handle);
    }

    Vector3 PhysicsBody2D::GetLinearVelocityPoint(const Vector3& localPoint) const
    {
        const b2Vec2 linearVelocity = b2Body_GetLocalPointVelocity(m_Handle, b2Vec2(localPoint.x, localPoint.y));
        return Vector3(ToVector2(linearVelocity));
    }

    void PhysicsBody2D::AddForce(const Vector3& force)
    {
        b2Body_ApplyForceToCenter(m_Handle, Tob2Vec2(force), true);
    }

    void PhysicsBody2D::AddImpulse(const Vector3& impulse)
    {
        b2Body_ApplyLinearImpulseToCenter(m_Handle, Tob2Vec2(impulse), true);
    }

    void PhysicsBody2D::AddForceAtPosition(const Vector3& position, const Vector3& force)
    {
        b2Body_ApplyForce(m_Handle, Tob2Vec2(force), Tob2Vec2(position), true);
    }

    void PhysicsBody2D::AddImpulseAtPosition(const Vector3& position, const Vector3& impulse)
    {
        b2Body_ApplyLinearImpulse(m_Handle, Tob2Vec2(impulse), Tob2Vec2(position), true);
    }

    const PhysicsConstraintsFlags& PhysicsBody2D::GetConstraints() const
    {
        return m_Constraints;
    }

    void PhysicsBody2D::SetConstraints(const PhysicsConstraintsFlags& constraints)
    {
        m_Constraints = constraints;
        b2Body_SetMotionLocks(m_Handle, Tob2MotionLocks(constraints));
    }

    const PhysicsMaterial& PhysicsBody2D::GetMaterial() const
    {
        return m_Material;
    }

    void PhysicsBody2D::SetMaterial(const PhysicsMaterial& material)
    {
        //b2SurfaceMaterial surfaceMaterial = b2DefaultSurfaceMaterial();
        //surfaceMaterial.friction = material.friction;
        //surfaceMaterial.restitution = material.bounciness;
        //b2Shape_SetSurfaceMaterial(m_Handle, surfaceMaterial);
    }

    PhysicsBodyType PhysicsBody2D::GetType() const
    {
        return m_Type;
    }

    void PhysicsBody2D::SetType(const PhysicsBodyType type)
    {
        m_Type = type;
    }

    bool PhysicsBody2D::IsSensor()
    {
        return false;
    }

    void PhysicsBody2D::SetIsSensor(bool isSensor)
    {
    }

    b2BodyId PhysicsBody2D::GetHandle() const
    {
        return m_Handle;
    }
}
