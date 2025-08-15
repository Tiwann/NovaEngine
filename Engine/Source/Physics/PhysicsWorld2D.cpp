#include "PhysicsWorld2D.h"
#include "PhysicsBody2D.h"
#include "PhysicsShape2D.h"
#include "PhysicsContactInfo.h"
#include "Box2DHelpers.h"

#include <box2d/box2d.h>

#include "Runtime/Application.h"


namespace Nova
{
    bool PhysicsWorld2D::Initialize(const PhysicsWorldCreateInfo& createInfo)
    {
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = b2Vec2(createInfo.gravity.x, createInfo.gravity.y);
        worldDef.userData = this;
        m_Handle = b2CreateWorld(&worldDef);
        if (!b2World_IsValid(m_Handle))
            return false;

        m_Owner = createInfo.scene;
        m_TimeStep = createInfo.timeStep;
        m_Iterations = createInfo.iterations;
        return true;
    }

    void PhysicsWorld2D::Step()
    {
        Application* application = GetApplication();
        b2World_Step(m_Handle, application->GetDeltaTime(), m_Iterations);

        const b2ContactEvents contactEvents = b2World_GetContactEvents(m_Handle);
        for (int i = 0; i < contactEvents.beginCount; ++i)
        {
            if (const b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + i;
                b2Shape_IsValid(beginEvent->shapeIdA) && b2Shape_IsValid(beginEvent->shapeIdB))
            {
                b2ContactData contactData = b2Contact_GetData(beginEvent->contactId);
                const PhysicsShape2D* shapeA = (PhysicsShape2D*)b2Shape_GetUserData(contactData.shapeIdA);
                const PhysicsShape2D* shapeB = (PhysicsShape2D*)b2Shape_GetUserData(contactData.shapeIdB);
                PhysicsBody2D* bodyA = shapeA->GetPhysicsBody();
                PhysicsBody2D* bodyB = shapeB->GetPhysicsBody();
                const b2Manifold& manifold = contactData.manifold;

                PhysicsContact contactA;
                contactA.point = Vector3(ToVector2(manifold.points[0].point));
                contactA.normal = Vector3(ToVector2(-manifold.normal));
                contactA.otherBody = bodyB;
                if (manifold.points[0].persisted) {
                    bodyA->onContactBeginEvent.Broadcast(contactA);
                } else{
                    bodyA->onContactStayEvent.Broadcast(contactA);
                }

                PhysicsContact contactB;
                contactB.point = Vector3(ToVector2(manifold.points[1].point));
                contactB.normal = Vector3(ToVector2(manifold.normal));
                contactB.otherBody = bodyA;
                if (manifold.points[1].persisted) {
                    bodyB->onContactBeginEvent.Broadcast(contactB);
                } else{
                    bodyB->onContactStayEvent.Broadcast(contactB);
                }
            }

        }

        for (int i = 0; i < contactEvents.endCount; ++i)
        {
            if (const b2ContactEndTouchEvent* endEvent = contactEvents.endEvents + i;
                b2Shape_IsValid(endEvent->shapeIdA) && b2Shape_IsValid(endEvent->shapeIdB))
            {
                b2ContactData contactData = b2Contact_GetData(endEvent->contactId);
                const PhysicsShape2D* shapeA = (PhysicsShape2D*)b2Shape_GetUserData(contactData.shapeIdA);
                const PhysicsShape2D* shapeB = (PhysicsShape2D*)b2Shape_GetUserData(contactData.shapeIdB);
                PhysicsBody2D* bodyA = shapeA->GetPhysicsBody();
                PhysicsBody2D* bodyB = shapeB->GetPhysicsBody();
                const b2Manifold& manifold = contactData.manifold;

                PhysicsContact contactA;
                contactA.point = Vector3(ToVector2(manifold.points[0].point));
                contactA.normal = Vector3(ToVector2(-manifold.normal));
                contactA.otherBody = bodyB;
                bodyA->onContactEndEvent.Broadcast(contactA);

                PhysicsContact contactB;
                contactB.point = Vector3(ToVector2(manifold.points[1].point));
                contactB.normal = Vector3(ToVector2(manifold.normal));
                contactB.otherBody = bodyA;
                bodyB->onContactEndEvent.Broadcast(contactB);
            }
        }

    }

    void PhysicsWorld2D::Destroy()
    {
        b2DestroyWorld(m_Handle);
    }

    PhysicsBody* PhysicsWorld2D::CreateBody(const PhysicsBodyDefinition& definition)
    {
        b2BodyDef def = b2DefaultBodyDef();
        def.position = b2Vec2(definition.position.x, definition.position.y);
        def.rotation = b2MakeRot(definition.rotation.ToEuler().z);
        def.type = (b2BodyType)definition.type;

        const b2BodyId bodyHandle = b2CreateBody(m_Handle, &def);
        PhysicsBody2D* createdBody = new PhysicsBody2D(bodyHandle, *this);
        b2Body_SetUserData(bodyHandle, this);
        m_Bodies.Add(createdBody);
        return createdBody;
    }

    void PhysicsWorld2D::DestroyBody(PhysicsBody* body)
    {
        b2DestroyBody(((PhysicsBody2D*)body)->GetHandle());
        m_Bodies.Remove(body);
        delete body;
    }

    void PhysicsWorld2D::SetGravity(const Vector3& gravity)
    {
        b2World_SetGravity(m_Handle, Tob2Vec2(gravity));
    }

    Vector3 PhysicsWorld2D::GetGravity() const
    {
        return Vector3(ToVector2(b2World_GetGravity(m_Handle)));
    }


    b2WorldId PhysicsWorld2D::GetHandle() const
    {
        return m_Handle;
    }
}
