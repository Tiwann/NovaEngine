#include "CircleShape2D.h"
#include "Box2DHelpers.h"
#include "PhysicsBody2D.h"
#include "PhysicsMaterial.h"
#include <box2d/box2d.h>

namespace Nova
{
    bool CircleShape2D::Initialize(PhysicsBody2D* body)
    {
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = m_Material->GetFriction();
        shapeDef.material.friction = m_Material->GetFriction();
        shapeDef.material.restitution = m_Material->GetBounciness();
        shapeDef.material.rollingResistance = 0;
        shapeDef.isSensor = m_IsSensor;
        shapeDef.enableContactEvents = true;
        shapeDef.enableSensorEvents = true;
        shapeDef.userData = this;

        if (b2Shape_IsValid(m_Shape)) b2DestroyShape(m_Shape, false);
        
        m_Shape = b2CreateCircleShape(body->GetHandle(), &shapeDef, &m_Circle);
        if (!b2Shape_IsValid(m_Shape)) return false;

        m_Body = body;
        return true;
    }

    void CircleShape2D::Destroy()
    {
        b2DestroyShape(m_Shape, true);
    }

    void CircleShape2D::SetPosition(const Vector2& position)
    {
        PhysicsShape2D::SetPosition(position);
        m_Circle.center = Tob2Vec2(position);
        if (b2Shape_IsValid(m_Shape)) b2Shape_SetCircle(m_Shape, &m_Circle);
    }

    void CircleShape2D::SetRadius(const float radius)
    {
        m_Radius = radius;
        m_Circle.radius = radius;
        if (b2Shape_IsValid(m_Shape)) b2Shape_SetCircle(m_Shape, &m_Circle);
    }

    float CircleShape2D::GetRadius() const
    {
        return m_Radius;
    }
}
