#include "PhysicsWorld3DContactListener.h"
#include "PhysicsWorld3D.h"
#include "PhysicsContact3D.h"
#include "PhysicsBody3D.h"
#include "JoltContact.h"
#include <Jolt/Physics/Body/BodyInterface.h>

namespace Nova
{
    PhysicsWorld3DContactListener::PhysicsWorld3DContactListener(PhysicsWorld3D* World) : m_World(World)
    {
    }

    // Validate all contacts by default
    JPH::ValidateResult PhysicsWorld3DContactListener::OnContactValidate(const JPH::Body& BodyHandleA, const JPH::Body& BodyHandleB, JPH::RVec3Arg BaseOffset, const JPH::CollideShapeResult& CollisionResult)
    {
        return JPH::ValidateResult::AcceptContact;
    }

    void PhysicsWorld3DContactListener::OnContactAdded(const JPH::Body& BodyHandleA, const JPH::Body& BodyHandleB, const JPH::ContactManifold& ContactManifold, JPH::ContactSettings& ContactSettings)
    {
        PhysicsBody3D* BodyA = (PhysicsBody3D*)BodyHandleA.GetUserData();
        PhysicsBody3D* BodyB = (PhysicsBody3D*)BodyHandleB.GetUserData();

        JPH::Contact ContactHandle{&BodyHandleA, &BodyHandleB, &ContactManifold, &ContactSettings};
        const PhysicsContact3D Contact = PhysicsContact3D(m_World, BodyA, BodyB, &ContactHandle);
        m_World->OnContactBegin(&Contact);
    }

    void PhysicsWorld3DContactListener::OnContactPersisted(const JPH::Body& BodyHandleA, const JPH::Body& BodyHandleB, const JPH::ContactManifold& ContactManifold, JPH::ContactSettings& ContactSettings)
    {
        PhysicsBody3D* BodyA = (PhysicsBody3D*)BodyHandleA.GetUserData();
        PhysicsBody3D* BodyB = (PhysicsBody3D*)BodyHandleB.GetUserData();

        JPH::Contact ContactHandle{&BodyHandleA, &BodyHandleB, &ContactManifold, &ContactSettings};
        const PhysicsContact3D Contact = PhysicsContact3D(m_World, BodyA, BodyB, &ContactHandle);
        m_World->OnContactStay(&Contact);
    }

    static PhysicsBody3D* GetBodyByID(const PhysicsWorld3D* World, const JPH::BodyID& BodyID)
    {
        const PhysicsWorld3D::BodyArray& Bodies = World->GetBodies();
        PhysicsBody3D** Body = Bodies.Single([&BodyID](const PhysicsBody3D* InBody) -> bool {
            const JPH::Body* Handle = InBody->GetHandle();
            return Handle->GetID() == BodyID;
        });
        return Body ? *Body : nullptr;
    }

    void PhysicsWorld3DContactListener::OnContactRemoved(const JPH::SubShapeIDPair& SubShapePair)
    {
        const JPH::BodyID& BodyA_ID = SubShapePair.GetBody1ID();
        const JPH::BodyID& BodyB_ID = SubShapePair.GetBody2ID();

        PhysicsBody3D* BodyA = GetBodyByID(m_World, BodyA_ID);
        PhysicsBody3D* BodyB = GetBodyByID(m_World, BodyB_ID);
        if(!BodyA || !BodyB) return;

        const PhysicsContact3D* Contact = new PhysicsContact3D(m_World, BodyA, BodyB, nullptr);
        m_World->OnContactEnd(Contact);
        delete Contact;
    }
}


