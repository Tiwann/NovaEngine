﻿#include "PhysicsWorld2D.h"
#include "PhysicsBody2D.h"
#include "PhysicsWorld2DContactListener.h"
#include "PhysicsContact2D.h"
#include "PhysicsContactInfo2D.h"
#include "Math/Functions.h"
#include "Math/MathConversions.h"
#include "Runtime/Application.h"

#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_contact.h>


namespace Nova
{
    void PhysicsWorld2D::OnInit(Scene* Owner)
    {
        m_World = new b2World({0.0f, -9.81f});
        m_ContactListener = new PhysicsWorld2DContactListener(this);
        m_World->SetContactListener(m_ContactListener);
    }

    void PhysicsWorld2D::Step()
    {
        m_World->Step(m_TimeStep * (f32)g_Application->GetTimeScale(), 8, 3);
    }

    void PhysicsWorld2D::OnDestroy()
    {
        delete m_ContactListener;
        delete m_World;
    }

    void PhysicsWorld2D::OnContactBegin(const PhysicsContact2D* Contact)
    {
        PhysicsBody2D* BodyA = Contact->BodyA;
        PhysicsBody2D* BodyB = Contact->BodyB;
        if(BodyA->IsSensor() || BodyB->IsSensor()) return;
            
        b2WorldManifold WorldManifold;
        Contact->Handle->GetWorldManifold(&WorldManifold);

        PhysicsContactInfo2D ContactInfoA;
        ContactInfoA.Point = ToVector2(WorldManifold.points[0]);
        ContactInfoA.Normal = ToVector2(-WorldManifold.normal);
        ContactInfoA.OtherBody = BodyB;
        BodyA->m_IsColliding = true;
        BodyA->OnContactBeginEvent.Broadcast(ContactInfoA);
        
        PhysicsContactInfo2D ContactInfoB;
        ContactInfoB.Point = ToVector2(WorldManifold.points[1]);
        ContactInfoB.Normal = ToVector2(WorldManifold.normal);
        ContactInfoB.OtherBody = BodyA;
        BodyB->m_IsColliding = true;
        BodyB->OnContactBeginEvent.Broadcast(ContactInfoB);
    }

    void PhysicsWorld2D::OnContactStay(const PhysicsContact2D* Contact)
    {
        PhysicsBody2D* BodyA = Contact->BodyA;
        PhysicsBody2D* BodyB = Contact->BodyB;
        if(BodyA->IsSensor() || BodyB->IsSensor()) return;
            
        b2WorldManifold WorldManifold;
        Contact->Handle->GetWorldManifold(&WorldManifold);

        PhysicsContactInfo2D ContactInfoA;
        ContactInfoA.Point = ToVector2(WorldManifold.points[0]);
        ContactInfoA.Normal = ToVector2(WorldManifold.normal);
        ContactInfoA.OtherBody = BodyB;
        BodyA->m_IsColliding = true;
        BodyA->OnContactStayEvent.Broadcast(ContactInfoA);
        
        PhysicsContactInfo2D ContactInfoB;
        ContactInfoB.Point = ToVector2(WorldManifold.points[1]);
        ContactInfoB.Normal = ToVector2(WorldManifold.normal);
        ContactInfoB.OtherBody = BodyA;
        BodyB->m_IsColliding = true;
        BodyB->OnContactStayEvent.Broadcast(ContactInfoB);
    }

    void PhysicsWorld2D::OnContactEnd(const PhysicsContact2D* Contact)
    {
        PhysicsBody2D* BodyA = Contact->BodyA;
        PhysicsBody2D* BodyB = Contact->BodyB;
        if(BodyA->IsSensor() || BodyB->IsSensor()) return;
            
        b2WorldManifold WorldManifold;
        Contact->Handle->GetWorldManifold(&WorldManifold);
                
        PhysicsContactInfo2D ContactInfoA;
        ContactInfoA.Point = ToVector2(WorldManifold.points[0]);
        ContactInfoA.Normal = ToVector2(-WorldManifold.normal);
        ContactInfoA.OtherBody = BodyB;
        BodyA->m_IsColliding = false;
        BodyA->OnContactEndEvent.Broadcast(ContactInfoA);

        PhysicsContactInfo2D ContactInfoB;
        ContactInfoB.Point = ToVector2(WorldManifold.points[1]);
        ContactInfoB.Normal = ToVector2(WorldManifold.normal);
        ContactInfoB.OtherBody = BodyA;
        BodyB->m_IsColliding = false;
        BodyB->OnContactEndEvent.Broadcast(ContactInfoB);
    }

    
    PhysicsBody2D* PhysicsWorld2D::CreateBody(const PhysicsBodyDefinition& Definition, const PhysicsMaterial& Material, PhysicsShape2D* Shape)
    {
        b2BodyDef BodyDefinition = {};
        BodyDefinition.position = ToB2Vec2(Vector2(Definition.Position));
        BodyDefinition.angle = Math::Radians(Definition.Rotation.z);
        BodyDefinition.type = (b2BodyType)Definition.Type;
        
        b2Body* BodyHandle = m_World->CreateBody(&BodyDefinition);
        PhysicsBody2D* CreatedBody = new PhysicsBody2D(BodyHandle, *this);
        BodyHandle->GetUserData().pointer = (uintptr_t)CreatedBody;
        m_Bodies.Add(CreatedBody);
        return CreatedBody;
    }

    void PhysicsWorld2D::DestroyBody(PhysicsBody2D* Body)
    {
        m_World->DestroyBody(Body->GetHandle());
        m_Bodies.Remove(Body);
        delete Body;
        Body = nullptr;
    }

    void PhysicsWorld2D::SetMaterial(PhysicsBody2D* Body, const PhysicsMaterial& Material)
    {
        Body->SetMaterial(Material);
    }

    void PhysicsWorld2D::SetGravity(const Vector2& Gravity)
    {
        m_World->SetGravity(ToB2Vec2(Gravity));
    }

    Vector2 PhysicsWorld2D::GetGravity() const
    {
        return ToVector2(m_World->GetGravity());
    }
}
