#pragma once
#include "PhysicsWorld.h"
#include "Math/Vector2.h"
#include <PhysicsExport.h>

class b2World;

namespace Nova
{
    class PhysicsBody2D;
    class PhysicsShape2D;
    class PhysicsWorld2DContactListener;
    struct PhysicsContact2D;
    
    class PHYSICS_API PhysicsWorld2D final : public PhysicsWorld<PhysicsBody2D, PhysicsShape2D, PhysicsContact2D>
    {
    public:
        void OnInit() override;
        void Step(f32 TimeStep) override;
        void OnDestroy() override;
        
        void OnContactBegin(const PhysicsContact2D* Contact) override;
        void OnContactStay(const PhysicsContact2D* Contact) override;
        void OnContactEnd(const PhysicsContact2D* Contact) override;
        
        PhysicsBody2D* CreateBody(const PhysicsBodyDefinition& Definition, const PhysicsMaterial& Material, PhysicsShape2D* Shape) override;
        void DestroyBody(PhysicsBody2D* Body) override;
        void SetMaterial(PhysicsBody2D* Body, const PhysicsMaterial& Material) override;

        void SetGravity(const Vector2& Gravity);
        Vector2 GetGravity() const;
    private:
        b2World* m_World = nullptr;
        PhysicsWorld2DContactListener* m_ContactListener = nullptr;
        Vector2 m_Gravity{ 0.0f, -9.81f };
    };
}
