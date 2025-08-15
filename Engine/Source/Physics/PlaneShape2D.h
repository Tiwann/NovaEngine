#pragma once
#include "PhysicsShape2D.h"
#include <box2d/collision.h>

namespace Nova
{
    class PlaneShape2D final : public PhysicsShape2D
    {
    public:
        bool Initialize(PhysicsBody2D* body) override;
        void Destroy() override;
        void SetPosition(const Vector2& position) override;
        void SetRotation(float rotation) override;
        void SetWidth(float width);
        float GetWidth() const;
    private:
        void UpdateSegment();
        float m_Width = 1.0f;
        b2Segment m_Segment = { };
    };
}
