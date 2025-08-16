#include "BoxShape2D.h"
#include "PhysicsBody2D.h"
#include "Box2DHelpers.h"
#include <box2d/box2d.h>

namespace Nova
{
    bool BoxShape2D::Initialize(PhysicsBody2D* body)
    {
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 1.0f;
        shapeDef.material.restitution = 0.0f;
        shapeDef.material.rollingResistance = 0;
        shapeDef.isSensor = m_IsSensor;
        shapeDef.enableContactEvents = true;
        shapeDef.enableSensorEvents = true;
        shapeDef.userData = this;

        if (b2Shape_IsValid(m_Shape)) b2DestroyShape(m_Shape, true);

        UpdateBox();
        m_Shape = b2CreatePolygonShape(body->GetHandle(), &shapeDef, &m_Polygon);
        if (!b2Shape_IsValid(m_Shape)) return false;

        m_Body = body;
        return true;
    }

    void BoxShape2D::Destroy()
    {
        b2DestroyShape(m_Shape, true);
    }

    void BoxShape2D::SetPosition(const Vector2& position)
    {
        PhysicsShape2D::SetPosition(position);
        UpdateBox();
    }

    void BoxShape2D::SetRotation(const float rotation)
    {
        PhysicsShape2D::SetRotation(rotation);
        UpdateBox();
    }

    float BoxShape2D::GetWidth() const
    {
        return m_Width;
    }

    void BoxShape2D::SetWidth(const float width)
    {
        m_Width = width;
        UpdateBox();
    }

    float BoxShape2D::GetHeight() const
    {
        return m_Height;
    }

    void BoxShape2D::SetHeight(const float height)
    {
        m_Height = height;
        UpdateBox();
    }

    void BoxShape2D::UpdateBox()
    {
        const b2Transform transform = b2Transform(Tob2Vec2(m_Position), b2MakeRot(m_Rotation));
        m_Polygon = b2MakeBox(m_Width * 0.5f, m_Height * 0.5f);
        m_Polygon = b2TransformPolygon(transform, &m_Polygon);
        if (b2Shape_IsValid(m_Shape))
            b2Shape_SetPolygon(m_Shape, &m_Polygon);
    }
}
