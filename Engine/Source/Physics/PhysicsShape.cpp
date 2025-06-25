#include "PhysicsShape.h"
#include "BoxShape2D.h"
#include "CircleShape2D.h"
#include "BoxShape3D.h"
#include "SphereShape3D.h"
#include "PlaneShape3D.h"


#include <box2d/b2_shape.h>
#include <box2d/b2_body.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>


namespace Nova
{
    
}

namespace Nova
{
    BoxShape2D::BoxShape2D(const Vector2& HalfExtents, const Vector2& Center, f32 Rotation)
        : m_HalfExtents(HalfExtents), m_Center(Center), m_Rotation(Rotation)
    {
        m_Shape = new b2PolygonShape();
        b2PolygonShape* AsPolygonShape = dynamic_cast<b2PolygonShape*>(m_Shape);
        const Vector3 Extents = Vector3(m_HalfExtents);
        AsPolygonShape->SetAsBox(Extents.x, Extents.y, { m_Center.x, m_Center.y }, m_Rotation);
    }

    CircleShape2D::CircleShape2D(f32 Radius, const Vector2& Center)
        : m_Radius(Radius), m_Center(Center)
    {
        m_Shape = new b2CircleShape();
        b2CircleShape* AsCircleShape = dynamic_cast<b2CircleShape*>(m_Shape);
        AsCircleShape->m_p = b2Vec2(m_Center.x, m_Center.y);
        AsCircleShape->m_radius = m_Radius;
    }
}

#define JPH_DISABLE_CUSTOM_ALLOCATOR
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/PlaneShape.h>

#include "Math/MathConversions.h"

namespace Nova
{
    BoxShape3D::BoxShape3D(Vector3 Center, Vector3 HalfExtents, Vector3 Rotation)
        : m_Center(Center), m_HalfExtents(HalfExtents), m_Rotation(Rotation)
    {
        m_Shape = new JPH::BoxShape(ToJoltVec3(m_HalfExtents));
    }

    SphereShape3D::SphereShape3D(f32 Radius) : m_Radius(Radius)
    {
        m_Shape = new JPH::SphereShape(m_Radius);
    }

    PlaneShape3D::PlaneShape3D(Vector3 Center, Quaternion Rotation) : m_Center(Center), m_Rotation(Rotation)
    {
        const Vector3 Normal = Rotation * Vector3::Up;
        const JPH::Plane Plane = JPH::Plane::sFromPointAndNormal(ToJoltVec3(m_Center), ToJoltVec3(Normal));
        m_Shape = new JPH::PlaneShape(Plane);
    }

}
