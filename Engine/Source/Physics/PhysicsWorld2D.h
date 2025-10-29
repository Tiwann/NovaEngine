#pragma once
#include "PhysicsWorld.h"
#include <box2d/id.h>

namespace Nova
{
    class PhysicsWorld2D : public PhysicsWorld
    {
    public:
        PhysicsWorld2D() : PhysicsWorld("Physics World 2D") {}
        bool Initialize(const PhysicsWorldCreateInfo& createInfo) override;
        void Step() override;
        void Destroy() override;

        PhysicsBody* CreateBody(const PhysicsBodyDefinition& definition) override;
        void DestroyBody(PhysicsBody* body) override;

        void SetGravity(const Vector3& gravity) override;
        Vector3 GetGravity() const override;

        b2WorldId GetHandle() const;
    private:
        b2WorldId m_Handle = b2_nullWorldId;
    };
}
