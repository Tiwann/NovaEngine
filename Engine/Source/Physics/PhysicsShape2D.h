#pragma once
#include "PhysicsShape.h"
#include "Math/Vector2.h"
#include <box2d/id.h>

namespace Nova
{
    class PhysicsBody2D;

    class PhysicsShape2D : public PhysicsShape
    {
    public:
        PhysicsShape2D() = default;
        virtual bool Initialize(PhysicsBody2D* body) = 0;
        virtual void Destroy() = 0;

        virtual void SetPosition(const Vector2& position);
        virtual Vector2 GetPosition() const;
        virtual void SetRotation(const float rotation);
        virtual float GetRotation() const;

        PhysicsBody2D* GetPhysicsBody() const;
        b2ShapeId GetHandle() const;

        void SetPhysicsMaterial(const Ref<PhysicsMaterial>& material) override;
        void SetIsSensor(bool isSensor) override;
    protected:
        friend PhysicsBody2D;
        PhysicsBody2D* m_Body = nullptr;
        b2ShapeId m_Shape = b2_nullShapeId;

        Vector2 m_Position;
        float m_Rotation = 0.0f;
    };
}
