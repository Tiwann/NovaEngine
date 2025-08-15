#pragma once
#include "PhysicsShape2D.h"
#include "Math/Vector2.h"
#include <box2d/collision.h>

namespace Nova
{
    class BoxShape2D final : public PhysicsShape2D
    {
    public:
        bool Initialize(PhysicsBody2D* body) override;
        void Destroy() override;

        void SetPosition(const Vector2& position) override;
        void SetRotation(float rotation) override;

        float GetWidth() const;
        void SetWidth(float width);
        float GetHeight() const;
        void SetHeight(float height);
    private:
        void UpdateBox();
        float m_Width = 1.0f, m_Height = 1.0f;
        b2Polygon m_Polygon = {};
    };
}
