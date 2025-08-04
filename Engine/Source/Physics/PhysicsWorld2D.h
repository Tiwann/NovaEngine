#pragma once
#include "PhysicsWorld.h"
#include <box2d/id.h>

namespace Nova
{
    class PhysicsWorld2D : public PhysicsWorld
    {
    public:
        bool Initialize(const PhysicsWorldCreateInfo& createInfo) override;
        void Step() override;
        void Destroy() override;

        PhysicsBody* CreateBody(const PhysicsBodyDefinition& definition, const PhysicsMaterial& material) override;
        void DestroyBody(PhysicsBody* body) override;

        b2WorldId GetHandle() const;
    private:
        b2WorldId m_Handle = b2_nullWorldId;
    };
}
