#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <PhysicsExport.h>

namespace Nova
{
    class PhysicsWorld3D;
    class PhysicsBody3D;
    
    class PHYSICS_HIDDEN PhysicsWorld3DContactListener : public JPH::ContactListener
    {
    public:
        PhysicsWorld3DContactListener(PhysicsWorld3D* World);
        JPH::ValidateResult OnContactValidate(const JPH::Body& BodyHandleA, const JPH::Body& BodyHandleB, JPH::RVec3Arg BaseOffset, const JPH::CollideShapeResult& CollisionResult) override;
        void OnContactAdded(const JPH::Body& BodyHandleA, const JPH::Body& BodyHandleB, const JPH::ContactManifold& ContactManifold, JPH::ContactSettings& ContactSettings) override;
        void OnContactPersisted(const JPH::Body& BodyHandleA, const JPH::Body& BodyHandleB, const JPH::ContactManifold& ContactManifold, JPH::ContactSettings& ContactSettings) override;
        void OnContactRemoved(const JPH::SubShapeIDPair& SubShapePair) override;

    private:
        PhysicsWorld3D* m_World = nullptr;
    };
}


