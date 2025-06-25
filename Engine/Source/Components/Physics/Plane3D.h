#pragma once
#include "RigidBody3D.h"

namespace Nova
{
    class Plane3D : public RigidBody3D
    {
    public:
        explicit Plane3D(Entity* Owner) : RigidBody3D(Owner, "Plane 3D")
        {
        }

        void RenderCollisions(Renderer* Renderer) const override;

        void OnInspectorGUI(const ImGuiIO& IO) override;
    protected:
        PhysicsShape3D* CreateShape(Transform* Transform) override;
    private:
        Vector3 m_Center = Vector3::Zero;
    };
}
