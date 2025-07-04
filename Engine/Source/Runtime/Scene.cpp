#include "Scene.h"
#include "Application.h"
#include "Entity.h"
#include "Components/Camera.h"
#include "Rendering/Renderer.h"


namespace Nova
{
    void Scene::OnInit()
    {
        m_PhysicsWorld2D.OnInit(this);
        m_PhysicsWorld3D.OnInit(this);
        
        for(Entity* Entity : m_Entities)
        {
            Entity->OnInit();
        }
    }

    void Scene::OnUpdate(f32 Delta)
    {
        for(Entity* Entity : m_Entities)
        {
            Entity->OnUpdate(Delta);
        }

        m_PhysicsWorld2D.Step();
        m_PhysicsWorld3D.Step();

        for(Entity* Entity : m_Entities)
        {
            Entity->OnPhysicsUpdate(Delta);
        }
    }

    void Scene::OnFrameBegin(Renderer* Renderer)
    {
        for(Entity* Entity : m_Entities)
        {
            Entity->OnFrameBegin(Renderer);
        }
    }

    void Scene::OnRender(Renderer* Renderer) const
    {
        if (Camera* Camera = Renderer->GetCurrentCamera())
        {
            Renderer->ClearColor(Camera->ClearColor);
        }
        for(Entity* Entity : m_Entities)
        {
            Entity->OnRender(Renderer);
        }
    }

    void Scene::OnDestroy()
    {
        for(const Entity* Entity : m_Entities)
        {
            EntityHandle Handle = Entity->GetGuid();
            DestroyEntity(Handle);
        }
        m_PhysicsWorld2D.OnDestroy();
        m_PhysicsWorld3D.OnDestroy();
    }

    EntityHandle Scene::CreateEntity(const String& Name)
    {
        Entity* Entity = m_EntityPool.New(Name, this);
        Entity->m_Enabled = true;
        Entity->OnInit();
        m_Entities.Add(Entity);
        return Entity->GetGuid();
    }

    bool Scene::DestroyEntity(EntityHandle& Handle)
    {
        Entity* Entity = Handle.GetEntity();
        if(!m_Entities.Contains(Entity)) return false;
        
        Entity->OnDestroy();
        m_Entities.Remove(Entity);
        m_EntityPool.Free(Entity);
        Handle = Entity::NullHandle;
        return true;
    }

    void Scene::ForEach(const Function<void(const EntityHandle&)>& Delegate)
    {
        for(Entity* Entity : m_Entities)
        {
            const EntityHandle Handle = Entity->GetGuid();
            Delegate.Call(Handle);
        }
    }

    void Scene::SetName(const String& Name)
    {
        m_Name = Name;
    }

    const PhysicsWorld2D& Scene::GetPhysicsWorld2D() const
    {
        return m_PhysicsWorld2D;
    }

    const PhysicsWorld3D& Scene::GetPhysicsWorld3D() const
    {
        return m_PhysicsWorld3D;
    }

    PhysicsWorld2D& Scene::GetPhysicsWorld2D()
    {
        return m_PhysicsWorld2D;
    }

    PhysicsWorld3D& Scene::GetPhysicsWorld3D()
    {
        return m_PhysicsWorld3D;
    }

    Scene::Iterator Scene::begin()
    {
        return m_Entities.begin();
    }

    Scene::Iterator Scene::end()
    {
        return m_Entities.end();
    }

    Scene::ConstIterator Scene::begin() const
    {
        return m_Entities.begin();
    }

    Scene::ConstIterator Scene::end() const
    {
        return m_Entities.end();
    }

    Array<Entity*> Scene::GetEntities()
    {
        return m_Entities;
    }
}
