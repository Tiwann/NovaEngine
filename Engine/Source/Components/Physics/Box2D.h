#pragma once
#include "RigidBody2D.h"
#include "Math/Vector2.h"

class b2Shape;

namespace Nova
{
    class Box2D : public RigidBody2D
    {
    public:
        Box2D(Entity* Owner);

        PhysicsShape2D* CreateShape(Transform* Transform) override;
        void OnInspectorGUI(const ImGuiIO& IO) override;
        
        Vector2 GetCenter() const;
        void SetCenter(const Vector2& Center);

        Vector2 GetHalfExtents() const;
        void SetHalfExtents(const Vector2& HalfExtents);

    protected:
        Vector2 m_Center = Vector2::Zero;
        Vector2 m_HalfExtents{0.5f, 0.5f};
        void RenderCollisions(Renderer* Renderer) const override;
    };
}

