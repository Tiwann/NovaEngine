#include "Box2D.h"
#include "Runtime/Scene.h"
#include "Components/Transform.h"
#include "Graphics/Renderer.h"
#include "Runtime/Color.h"
#include "Physics/PhysicsBody2D.h"
#include "Physics/BoxShape2D.h"
#include "Editor/EditorGUI.h"

static constexpr char ComponentName[7] = "Box 2D";

namespace Nova
{
    Box2D::Box2D(Entity* Owner) : RigidBody2D(Owner, ComponentName)
    {
        
    }

    PhysicsShape2D* Box2D::CreateShape(Transform* Transform)
    {
        const Matrix4 TransformMat = Transform->GetWorldSpaceMatrix();
        const Vector2 TransformedHalfExtents = TransformMat * Vector3(m_HalfExtents);
        return new BoxShape2D(TransformedHalfExtents, m_Center, 0.0f);
    }

    void Box2D::OnInspectorGUI(const ImGuiIO& IO)
    {
        UI::DragVector2<f32>("Center", m_Center, 0.01f);
        UI::DragVector2<f32>("Half Extents", m_HalfExtents, 0.01f);
        RigidBody2D::OnInspectorGUI(IO);
    }


    Vector2 Box2D::GetCenter() const
    {
        return m_Center;
    }

    void Box2D::SetCenter(const Vector2& Center)
    {
        m_Center = Center;
    }

    Vector2 Box2D::GetHalfExtents() const
    {
        return m_HalfExtents;
    }

    void Box2D::SetHalfExtents(const Vector2& HalfExtents)
    {
        m_HalfExtents = HalfExtents;
    }
    
    void Box2D::RenderCollisions(Renderer* Renderer) const
    {
        const Matrix4 Transform = GetTransform()->GetWorldSpaceMatrix();
        Renderer->DrawWireQuad(Transform, m_Center, m_HalfExtents, 3.0f, Color::Green);
    }
}
