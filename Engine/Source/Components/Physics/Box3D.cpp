#include "Box3D.h"
#include "Components/Transform.h"
#include "Runtime/Color.h"
#include "Rendering/Renderer.h"
#include "Physics/BoxShape3D.h"
#include "Editor/EditorGUI.h"
#include "Math/Matrix4.h"

namespace Nova
{
    Box3D::Box3D(Entity* Owner) : RigidBody3D(Owner, "Box 3D")
    {
        
    }

    void Box3D::OnInspectorGUI(const ImGuiIO& IO)
    {
        UI::DragVector3<f32>("Center", m_Center, 0.01f);
        UI::DragVector3<f32>("Half Extents", m_HalfExtents, 0.01f);
        UI::DragVector3<f32>("Rotation", m_Rotation, 0.01f);
        RigidBody3D::OnInspectorGUI(IO);
    }

    PhysicsShape3D* Box3D::CreateShape(Transform* Transform)
    {
        const Matrix4 TransformMat = Transform->GetWorldSpaceMatrix();
        const Vector3 TransformedHalfExtents = TransformMat * m_HalfExtents;
        return new BoxShape3D(m_Center, TransformedHalfExtents, m_Rotation);
    }

    Vector3 Box3D::GetCenter() const
    {
        return m_Center;
    }

    void Box3D::SetCenter(const Vector3& center)
    {
        m_Center = center;
    }

    Vector3 Box3D::GetHalfExtents() const
    {
        return m_HalfExtents;
    }

    void Box3D::SetHalfExtents(const Vector3& half_extents)
    {
        m_HalfExtents = half_extents;
    }

    void Box3D::RenderCollisions(Renderer* Renderer) const
    {
        const Matrix4 Transform = GetTransform()->GetWorldSpaceMatrix();
        Renderer->DrawWireQuad(Transform, m_Center, m_HalfExtents, 1.0f, Color::Green);
    }
}
