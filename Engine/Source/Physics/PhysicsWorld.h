#pragma once
#include "PhysicsBodyDefinition.h"
#include "Containers/Array.h"
#include "Math/Vector3.h"
#include "Runtime/Object.h"
#include "Runtime/Scene.h"

namespace Nova
{
    class Application;
    class Scene;
    class PhysicsBody;
    class PhysicsShape;

    struct PhysicsWorldCreateInfo
    {
        Scene* scene = nullptr;
        Vector3 gravity = Vector3(0.0f, -9.81f, 0.0f);
        float timeStep = 1.0f / 60.0f;
        uint32_t iterations = 4;
    };

    class PhysicsWorld : public Object
    {
    public:
        PhysicsWorld() = default;
        ~PhysicsWorld() override = default;
        virtual bool Initialize(const PhysicsWorldCreateInfo& createInfo) = 0;
        virtual void Step() = 0;
        virtual void Destroy() = 0;
        
        virtual PhysicsBody* CreateBody(const PhysicsBodyDefinition& definition) = 0;
        virtual void DestroyBody(PhysicsBody* body) = 0;

        virtual void SetGravity(const Vector3& gravity) = 0;
        virtual Vector3 GetGravity() const = 0;

        Scene* GetOwner() { return m_Owner;}
        Application* GetApplication() { return m_Owner->GetOwner(); }
    protected:
        Scene* m_Owner = nullptr;
        Array<PhysicsBody*> m_Bodies;
        float m_TimeStep = 0.0f;
        uint32_t m_Iterations = 0;
    };

}
