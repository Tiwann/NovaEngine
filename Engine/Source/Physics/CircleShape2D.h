#pragma once
#include "PhysicsShape2D.h"
#include <box2d/collision.h>

namespace Nova
{
    class CircleShape2D final : public PhysicsShape2D
    {
    public:
        bool Initialize(PhysicsBody2D* body) override;
        void Destroy() override;

        void SetPosition(const Vector2& position) override;
        void SetRadius(float radius);
        float GetRadius() const;
    private:
        b2Circle m_Circle = { };
        float m_Radius = 1.0f;
    };
}