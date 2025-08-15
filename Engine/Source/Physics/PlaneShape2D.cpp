#include "PlaneShape2D.h"
#include "PhysicsBody2D.h"
#include <box2d/box2d.h>


namespace Nova
{
    bool PlaneShape2D::Initialize(PhysicsBody2D* body)
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

        if (b2Shape_IsValid(m_Shape))
            b2DestroyShape(m_Shape, true);

        UpdateSegment();
        m_Shape = b2CreateSegmentShape(body->GetHandle(), &shapeDef, &m_Segment);
        if (!b2Shape_IsValid(m_Shape)) return false;

        m_Body = body;
        return true;
    }

    void PlaneShape2D::Destroy()
    {
        b2DestroyShape(m_Shape, true);
    }

    void PlaneShape2D::SetPosition(const Vector2& position)
    {
        PhysicsShape2D::SetPosition(position);
        UpdateSegment();
    }

    void PlaneShape2D::SetRotation(const float rotation)
    {
        PhysicsShape2D::SetRotation(rotation);
        UpdateSegment();
    }

    void PlaneShape2D::SetWidth(const float width)
    {
        m_Width = width;
        UpdateSegment();
    }

    float PlaneShape2D::GetWidth() const
    {
        return m_Width;
    }

    void PlaneShape2D::UpdateSegment()
    {
        const b2Vec2 point1 = b2Vec2(m_Position.x - m_Width * 0.5f, m_Position.y);
        const b2Vec2 point2 = b2Vec2(m_Position.x + m_Width * 0.5f, m_Position.y);
        m_Segment.point1 = b2RotateVector(b2MakeRot(m_Rotation), point1);
        m_Segment.point2 = b2RotateVector(b2MakeRot(m_Rotation), point2);
    }
}
