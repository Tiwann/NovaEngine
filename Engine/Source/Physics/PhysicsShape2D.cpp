#include "PhysicsShape2D.h"
#include "PhysicsMaterial.h"
#include <box2d/box2d.h>


namespace Nova
{
    void PhysicsShape2D::SetPosition(const Vector2& position)
    {
        m_Position = position;
    }

    Vector2 PhysicsShape2D::GetPosition() const
    {
        return m_Position;
    }

    void PhysicsShape2D::SetRotation(const float rotation)
    {
        m_Rotation = rotation;
    }

    float PhysicsShape2D::GetRotation() const
    {
        return m_Rotation;
    }

    PhysicsBody2D* PhysicsShape2D::GetPhysicsBody() const
    {
        return m_Body;
    }

    b2ShapeId PhysicsShape2D::GetHandle() const
    {
        return m_Shape;
    }

    void PhysicsShape2D::SetPhysicsMaterial(const Ref<PhysicsMaterial>& material)
    {
        PhysicsShape::SetPhysicsMaterial(material);
        if (b2Shape_IsValid(m_Shape))
        {
            b2SurfaceMaterial surfaceMat = b2Shape_GetSurfaceMaterial(m_Shape);
            surfaceMat.friction = m_Material->GetFriction();
            surfaceMat.restitution = m_Material->GetBounciness();
            b2Shape_SetSurfaceMaterial(m_Shape, &surfaceMat);
            b2Shape_SetDensity(m_Shape, material->GetDensity(), true);
        }
    }

    void PhysicsShape2D::SetIsSensor(const bool isSensor)
    {
        PhysicsShape::SetIsSensor(isSensor);
        if (b2Shape_IsValid(m_Shape))
            Initialize(m_Body);
    }
}
