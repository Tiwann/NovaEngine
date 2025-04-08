#pragma once
#include "RigidBody2D.h"
#include "Math/Vector2.h"

namespace Nova
{
    class Circle2D : public RigidBody2D
    {
    public:
        Circle2D(Entity* Owner);
        void OnInspectorGUI(const ImGuiIO& IO) override;
        PhysicsShape2D* CreateShape(Transform* Transform) override;
        
        Vector2 GetCenter() const;
        void SetCenter(const Vector2& Center);

        f32 GetRadius() const;
        void SetRadius(f32 radius);
        
    
    protected:
        Vector2 m_Center = Vector2::Zero;
        f32 m_Radius = 1.0f;
        void RenderCollisions(Renderer* Renderer) const override;
    };
}
