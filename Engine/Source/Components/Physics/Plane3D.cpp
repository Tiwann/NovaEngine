#include "Plane3D.h"

#include "Components/Transform.h"
#include "Physics/PlaneShape3D.h"
#include "Editor/EditorGUI.h"

namespace Nova
{
    void Plane3D::RenderCollisions(Renderer* Renderer) const
    {
    }

    void Plane3D::OnInspectorGUI(const ImGuiIO& IO)
    {
        UI::DragVector3<f32>("Center", m_Center);
        RigidBody3D::OnInspectorGUI(IO);
    }

    PhysicsShape3D* Plane3D::CreateShape(Transform* Transform)
    {
        return new PlaneShape3D(m_Center, Transform->GetRotation());
    }
}
