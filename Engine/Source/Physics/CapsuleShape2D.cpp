#include "CapsuleShape2D.h"
#include "PhysicsBody2D.h"
#include <box2d/box2d.h>

namespace Nova
{
    bool CapsuleShape2D::Initialize(PhysicsBody2D* body)
    {
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 1.0f;
        shapeDef.material.friction = 0.0f;
        shapeDef.material.restitution = 0.0f;
        shapeDef.material.rollingResistance = 0;
        shapeDef.isSensor = m_IsSensor;
        shapeDef.enableContactEvents = true;
        shapeDef.enableSensorEvents = true;
        shapeDef.userData = this;

        if (b2Shape_IsValid(m_Shape)) b2DestroyShape(m_Shape, true);

        UpdateCapsule();
        m_Shape = b2CreateCapsuleShape(body->GetHandle(), &shapeDef, &m_Capsule);
        if (!b2Shape_IsValid(m_Shape)) return false;

        m_Body = body;
        return true;
    }

    void CapsuleShape2D::Destroy()
    {
        b2DestroyShape(m_Shape, true);
    }

    void CapsuleShape2D::SetRadius(const float radius)
    {
        m_Radius = radius;
        UpdateCapsule();
        if (b2Shape_IsValid(m_Shape))
            b2Shape_SetCapsule(m_Shape, &m_Capsule);
    }

    void CapsuleShape2D::SetHeight(const float height)
    {
        m_Height = height;
        UpdateCapsule();
        if (b2Shape_IsValid(m_Shape))
            b2Shape_SetCapsule(m_Shape, &m_Capsule);
    }

    float CapsuleShape2D::GetRadius() const
    {
        return m_Radius;
    }

    float CapsuleShape2D::GetHeight() const
    {
        return m_Height;
    }

    void CapsuleShape2D::UpdateCapsule()
    {
        const b2Vec2 center1 = b2Vec2(m_Position.x, m_Position.y + m_Height * 0.5f);
        const b2Vec2 center2 = b2Vec2(m_Position.x, m_Position.y - m_Height * 0.5f);
        m_Capsule.center1 = b2RotateVector(b2MakeRot(m_Rotation), center1);
        m_Capsule.center2 = b2RotateVector(b2MakeRot(m_Rotation), center2);
        m_Capsule.radius = m_Radius;
    }
}
