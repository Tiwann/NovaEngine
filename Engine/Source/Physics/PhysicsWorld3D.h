#pragma once
#include "PhysicsWorld.h"

#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <CoreExport.h>

namespace Nova
{
    class PhysicsBody3D;
    class PhysicsShape3D;
    struct PhysicsContact3D;
    class PhysicsWorld3DContactListener;

    class PHYSICS_HIDDEN PhysicsWorld3DBroadPhaseLayerInterface : public JPH::BroadPhaseLayerInterface
    {
    public:
        JPH::uint GetNumBroadPhaseLayers() const override;
        JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override;
    };

    class PHYSICS_HIDDEN PhysicsWorld3DObjectVsBroadPhaseLayerFilter : public JPH::ObjectVsBroadPhaseLayerFilter
    {
    public:
        bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override;
    };

    class PHYSICS_HIDDEN PhysicsWorld3DObjectLayerPairFilter : public JPH::ObjectLayerPairFilter
    {
    public:
        bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::ObjectLayer inLayer2) const override;
    };
    
    class CORE_API PhysicsWorld3D : public PhysicsWorld<PhysicsBody3D, PhysicsShape3D, PhysicsContact3D>
    {
    public:
        PhysicsWorld3D();
        void OnInit(Scene* Owner) override;
        void Step(f32 TimeStep) override;
        void OnDestroy() override;

        void OnContactBegin(const PhysicsContact3D* Contact) override;
        void OnContactStay(const PhysicsContact3D* Contact) override;
        void OnContactEnd(const PhysicsContact3D* Contact) override;

        PhysicsBody3D* CreateBody(const PhysicsBodyDefinition& Definition, const PhysicsMaterial& Material, PhysicsShape3D* Shape) override;
        void DestroyBody(PhysicsBody3D* Body) override;
        void SetMaterial(PhysicsBody3D* Body, const PhysicsMaterial& Material) override;

        const JPH::PhysicsSystem& GetSystem() const;
        JPH::PhysicsSystem& GetSystem();

    private:
        JPH::TempAllocatorImpl m_TempAllocator;
        JPH::JobSystemThreadPool m_JobSystem;
        JPH::PhysicsSystem m_System;
        PhysicsWorld3DBroadPhaseLayerInterface m_LayerInterface;
        PhysicsWorld3DObjectVsBroadPhaseLayerFilter m_LayerFilter;
        PhysicsWorld3DObjectLayerPairFilter m_LayerPairFilter;
        PhysicsWorld3DContactListener* m_ContactListener = nullptr;

        static constexpr u32 MaxBodies = 0xFFFFui32;
        static constexpr u32 MaxBodyPairs = 0xFFFFui32;
        static constexpr u32 MaxContactConstraints = 10240;
    };
}
