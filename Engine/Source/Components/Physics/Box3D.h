#pragma once
#include "RigidBody3D.h"
#include "Math/Vector3.h"

namespace Nova
{
    
    class Box3D : public RigidBody3D
    {
    public:
        Box3D(Entity* Owner);
        void OnInspectorGUI(const ImGuiIO& IO) override;
        PhysicsShape3D* CreateShape(Transform* Transform) override;

        Vector3 GetCenter() const;
        void SetCenter(const Vector3& center);
        Vector3 GetHalfExtents() const;
        void SetHalfExtents(const Vector3& half_extents);

    protected:
        Vector3 m_Center = Vector3::Zero;
        Vector3 m_HalfExtents = { 0.5f, 0.5f, 0.5f };
        Vector3 m_Rotation;
        void RenderCollisions(Renderer* Renderer) const override;
    };
}
