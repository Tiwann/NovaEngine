#include "PhysicsWorld2DContactListener.h"
#include "PhysicsWorld2D.h"
#include "PhysicsContact2D.h"
#include <box2d/b2_contact.h>

namespace Nova
{
    PhysicsWorld2DContactListener::PhysicsWorld2DContactListener(PhysicsWorld2D* World) : m_World(World)
    {
    }

    void PhysicsWorld2DContactListener::BeginContact(b2Contact* contact)
    {
        if(!contact->IsTouching())
            return;
        
        PhysicsBody2D* BodyA = (PhysicsBody2D*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
        PhysicsBody2D* BodyB = (PhysicsBody2D*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
        
        const PhysicsContact2D Contact = PhysicsContact2D(m_World, BodyA, BodyB, contact);
        m_World->OnContactBegin(&Contact);
    }

    void PhysicsWorld2DContactListener::PersistContact(b2Contact* contact)
    {
        if(!contact->IsTouching())
            return;
        
        PhysicsBody2D* BodyA = (PhysicsBody2D*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
        PhysicsBody2D* BodyB = (PhysicsBody2D*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
        
        const PhysicsContact2D Contact = PhysicsContact2D(m_World, BodyA, BodyB, contact);
        m_World->OnContactStay(&Contact);
    }

    void PhysicsWorld2DContactListener::EndContact(b2Contact* contact)
    {
        if(contact->IsTouching())
            return;
        
        PhysicsBody2D* BodyA = (PhysicsBody2D*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
        PhysicsBody2D* BodyB = (PhysicsBody2D*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
        
        const PhysicsContact2D Contact = PhysicsContact2D(m_World, BodyA, BodyB, contact);
        m_World->OnContactEnd(&Contact);
    }
}


