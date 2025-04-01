#pragma once
#include <box2d/b2_world_callbacks.h>
#include <PhysicsExport.h>

namespace Nova
{
    class PhysicsWorld2D;
    
    class PHYSICS_HIDDEN PhysicsWorld2DContactListener : public b2ContactListener
    {
    public:
        PhysicsWorld2DContactListener(PhysicsWorld2D* World);
        void BeginContact(b2Contact* contact) override;
        void PersistContact(b2Contact* contact) override;
        void EndContact(b2Contact* contact) override;

    private:
        PhysicsWorld2D* m_World = nullptr;
    };
}


