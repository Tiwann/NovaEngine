#include "PhysicsWorld3D.h"
#include "Physics3D.h"
#include "PhysicsBody3D.h"
#include "PhysicsMaterial.h"
#include "PhysicsWorld3DContactListener.h"
#include "Math/Functions.h"

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include "JoltContact.h"
#include "PhysicsContact3D.h"
#include "PhysicsContactInfo3D.h"
#include "PhysicsShape3D.h"
#include "Core/Application.h"


namespace Nova
{
    JPH::uint PhysicsWorld3DBroadPhaseLayerInterface::GetNumBroadPhaseLayers() const
    {
        return 1;
    }

    JPH::BroadPhaseLayer PhysicsWorld3DBroadPhaseLayerInterface::GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const
    {
        return {};
    }

    bool PhysicsWorld3DObjectVsBroadPhaseLayerFilter::ShouldCollide(JPH::ObjectLayer inLayer1,
        JPH::BroadPhaseLayer inLayer2) const
    {
        return ObjectVsBroadPhaseLayerFilter::ShouldCollide(inLayer1, inLayer2);
    }

    bool PhysicsWorld3DObjectLayerPairFilter::ShouldCollide(JPH::ObjectLayer inLayer1, JPH::ObjectLayer inLayer2) const
    {
        return ObjectLayerPairFilter::ShouldCollide(inLayer1, inLayer2);
    }

    PhysicsWorld3D::PhysicsWorld3D()
    : m_TempAllocator(10 * 1024 * 1024),
    m_JobSystem(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, (int)(std::thread::hardware_concurrency() - 1))
    {
        
    }

    void PhysicsWorld3D::OnInit()
    {
        JPH::RegisterDefaultAllocator();
        JPH::Factory::sInstance = new JPH::Factory();
        JPH::RegisterTypes();
        
        m_System.SetGravity(Physics3D::Gravity);
        
        m_System.Init(Physics3D::MaxBodies,
            0,
            Physics3D::MaxBodyPairs,
            Physics3D::MaxContactConstraints,
            m_LayerInterface,
            m_LayerFilter,
            m_LayerPairFilter);

        m_ContactListener = new PhysicsWorld3DContactListener(this);
        m_System.SetContactListener(m_ContactListener);
    }

    void PhysicsWorld3D::Step(const f32 TimeStep)
    {
        m_System.Update(TimeStep * (f32)g_Application->GetTimeScale(), 2, &m_TempAllocator, &m_JobSystem);
    }

    void PhysicsWorld3D::OnDestroy()
    {
        m_System.SetContactListener(nullptr);
        delete m_ContactListener;
        JPH::UnregisterTypes();
        
        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;
    }

    void PhysicsWorld3D::OnContactBegin(const PhysicsContact3D* Contact)
    {
        PhysicsBody3D* BodyA = Contact->BodyA;
        PhysicsBody3D* BodyB = Contact->BodyB;
        if(BodyA->IsSensor() || BodyB->IsSensor()) return;

        
        const JPH::ContactManifold& WorldManifold = *Contact->Handle->Manifold;

        PhysicsContactInfo3D ContactInfoA;
        ContactInfoA.Point = WorldManifold.GetWorldSpaceContactPointOn1(0);
        ContactInfoA.Normal = -WorldManifold.mWorldSpaceNormal;
        ContactInfoA.OtherBody = BodyB;
        BodyA->m_IsColliding = true;
        BodyA->OnContactBeginEvent.Broadcast(ContactInfoA);
        
        PhysicsContactInfo3D ContactInfoB;
        ContactInfoB.Point = WorldManifold.GetWorldSpaceContactPointOn2(0);
        ContactInfoB.Normal = WorldManifold.mWorldSpaceNormal;
        ContactInfoB.OtherBody = BodyA;
        BodyB->m_IsColliding = true;
        BodyB->OnContactBeginEvent.Broadcast(ContactInfoB);
    }

    void PhysicsWorld3D::OnContactStay(const PhysicsContact3D* Contact)
    {
        PhysicsBody3D* BodyA = Contact->BodyA;
        PhysicsBody3D* BodyB = Contact->BodyB;
        if(BodyA->IsSensor() || BodyB->IsSensor()) return;

        
        const JPH::ContactManifold& WorldManifold = *Contact->Handle->Manifold;

        PhysicsContactInfo3D ContactInfoA;
        ContactInfoA.Point = WorldManifold.GetWorldSpaceContactPointOn1(0);
        ContactInfoA.Normal = -WorldManifold.mWorldSpaceNormal;
        ContactInfoA.OtherBody = BodyB;
        BodyA->m_IsColliding = true;
        BodyA->OnContactBeginEvent.Broadcast(ContactInfoA);
        
        PhysicsContactInfo3D ContactInfoB;
        ContactInfoB.Point = WorldManifold.GetWorldSpaceContactPointOn2(0);
        ContactInfoB.Normal = WorldManifold.mWorldSpaceNormal;
        ContactInfoB.OtherBody = BodyA;
        BodyB->m_IsColliding = true;
        BodyB->OnContactBeginEvent.Broadcast(ContactInfoB);
    }

    void PhysicsWorld3D::OnContactEnd(const PhysicsContact3D* Contact)
    {
        PhysicsBody3D* BodyA = Contact->BodyA;
        PhysicsBody3D* BodyB = Contact->BodyB;
        if(BodyA->IsSensor() || BodyB->IsSensor()) return;

        
        const JPH::ContactManifold& WorldManifold = *Contact->Handle->Manifold;

        PhysicsContactInfo3D ContactInfoA;
        ContactInfoA.Point = WorldManifold.GetWorldSpaceContactPointOn1(0);
        ContactInfoA.Normal = -WorldManifold.mWorldSpaceNormal;
        ContactInfoA.OtherBody = BodyB;
        BodyA->m_IsColliding = false;
        BodyA->OnContactBeginEvent.Broadcast(ContactInfoA);
        
        PhysicsContactInfo3D ContactInfoB;
        ContactInfoB.Point = WorldManifold.GetWorldSpaceContactPointOn2(0);
        ContactInfoB.Normal = WorldManifold.mWorldSpaceNormal;
        ContactInfoB.OtherBody = BodyA;
        BodyB->m_IsColliding = false;
        BodyB->OnContactBeginEvent.Broadcast(ContactInfoB);
    }

    PhysicsBody3D* PhysicsWorld3D::CreateBody(const PhysicsBodyDefinition& Definition, const PhysicsMaterial& Material, PhysicsShape3D* Shape)
    {
        JPH::BodyInterface& BodyInterface = m_System.GetBodyInterface();
        JPH::BodyCreationSettings Settings;
        Settings.mPosition = Definition.Position;
        Settings.mRotation = JPH::Quat::sEulerAngles(Definition.Rotation.Apply(Math::Radians));
        Settings.mMotionType = (JPH::EMotionType)Definition.Type;
        Settings.mFriction = Material.Friction;
        Settings.mRestitution = Material.Bounciness;
        Settings.mIsSensor = Definition.IsTrigger;
        Settings.mAllowDynamicOrKinematic = true;
        
        const JPH::Shape& ShapeHandle = Shape->GetHandle();
        Settings.SetShape(&ShapeHandle);
        
        JPH::Body* BodyHandle = BodyInterface.CreateBody(Settings);
        BodyInterface.AddBody(BodyHandle->GetID(), JPH::EActivation::Activate);
        PhysicsBody3D* CreatedBody = new PhysicsBody3D(BodyHandle, *this);
        m_Bodies.Add(CreatedBody);
        return CreatedBody;
    }

    void PhysicsWorld3D::DestroyBody(PhysicsBody3D* Body)
    {
        JPH::BodyInterface& BodyInterface = m_System.GetBodyInterface();
        const JPH::Body* BodyHandle = Body->GetHandle();
        const JPH::BodyID& BodyId = BodyHandle->GetID();
        BodyInterface.RemoveBody(BodyId);
        BodyInterface.DestroyBody(BodyId);
        m_Bodies.Remove(Body);
        delete Body;
        Body = nullptr;
    }

    void PhysicsWorld3D::SetMaterial(PhysicsBody3D* Body, const PhysicsMaterial& Material)
    {
        Body->SetMaterial(Material);
    }

    const JPH::PhysicsSystem& PhysicsWorld3D::GetSystem() const
    {
        return m_System;
    }

    JPH::PhysicsSystem& PhysicsWorld3D::GetSystem()
    {
        return m_System;
    }
}

