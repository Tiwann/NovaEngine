#pragma once
#include "Runtime/Component.h"
#include "Physics/PhysicsBody.h"

namespace Nova
{
    class PhysicsComponent : public Component
    {
    public:
        PhysicsComponent(Entity* owner, const String& name) : Component(owner, name) {}

        void OnPhysicsUpdate(float deltaTime) override;
        PhysicsBody* GetPhysicsBody();
        const PhysicsBody* GetPhysicsBody() const;

    protected:
        PhysicsBody* m_Body = nullptr;
    };
}