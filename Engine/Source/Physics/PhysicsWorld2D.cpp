#include "PhysicsWorld2D.h"
#include "PhysicsBody2D.h"
#include <box2d/box2d.h>

namespace Nova
{
    bool PhysicsWorld2D::Initialize(const PhysicsWorldCreateInfo& createInfo)
    {
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = b2Vec2(createInfo.gravity.x, createInfo.gravity.y);
        worldDef.userData = this;
        m_Handle = b2CreateWorld(&worldDef);
        if (Memory::Memcmp(&m_Handle, &b2_nullWorldId, sizeof(b2WorldId)))
            return false;

        m_Owner = createInfo.scene;
        m_TimeStep = createInfo.timeStep;
        m_Iterations = createInfo.iterations;
        return true;
    }

    void PhysicsWorld2D::Step()
    {
        b2World_Step(m_Handle, m_TimeStep, m_Iterations);
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
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = definition.material.density;
        shapeDef.material.friction = definition.material.friction;
        shapeDef.material.restitution = definition.material.bounciness;
        shapeDef.material.rollingResistance = 0;
        shapeDef.isSensor = definition.isTrigger;
        shapeDef.enableContactEvents = true;
        shapeDef.enableSensorEvents = true;

        const b2Polygon dynamicBox = b2MakeBox(1.0f, 1.0f);
        b2CreatePolygonShape(bodyHandle, &shapeDef, &dynamicBox);


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


    b2WorldId PhysicsWorld2D::GetHandle() const
    {
        return m_Handle;
    }
}
