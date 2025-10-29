#pragma once
#include "PhysicsShape2D.h"
#include <box2d/collision.h>

namespace Nova
{
    class CapsuleShape2D final : public PhysicsShape2D
    {
    public:
        bool Initialize(PhysicsBody2D* body) override;
        void Destroy() override;

        void SetRadius(float radius);
        void SetHeight(float height);
        float GetRadius() const;
        float GetHeight() const;
    private:
        void UpdateCapsule();
        float m_Height = 1.0f;
        float m_Radius = 0.25f;
        b2Capsule m_Capsule = { };
    };
}
