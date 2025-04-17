#include "Circle2D.h"
#include "Components/Transform.h"
#include "Runtime/Color.h"
#include "Rendering/Renderer.h"
#include "Physics/PhysicsBody2D.h"
#include "Physics/CircleShape2D.h"
#include "Editor/EditorGUI.h"


namespace Nova
{
    Circle2D::Circle2D(Entity* Owner) : RigidBody2D(Owner, "Circle 2D")
    {
        
    }

    void Circle2D::OnInspectorGUI(const ImGuiIO& IO)
    {
        UI::DragVector2<f32>("Center", m_Center);
        UI::DragValue<f32>("Radius", m_Radius);
        RigidBody2D::OnInspectorGUI(IO);
    }

    PhysicsShape2D* Circle2D::CreateShape(Transform* Transform)
    {
        return new CircleShape2D(m_Radius, m_Center);
    }

    Vector2 Circle2D::GetCenter() const
    {
        return m_Center;
    }

    void Circle2D::SetCenter(const Vector2& Center)
    {
        m_Center = Center;
    }

    f32 Circle2D::GetRadius() const
    {
        return m_Radius;
    }

    void Circle2D::SetRadius(f32 radius)
    {
        m_Radius = radius;
    }
    
    void Circle2D::RenderCollisions(Renderer* Renderer) const
    {
        const Matrix4 Transform = GetTransform()->GetWorldSpaceMatrix();
        // Renderer->DrawCircle(Transform, m_Center, m_Radius, Color::Green);
    }
}
