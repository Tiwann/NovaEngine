#include "Scene.h"
#include "Entity.h"

#ifdef NOVA_HAS_PHYSICS
#include "Physics/PhysicsWorld2D.h"
#endif

namespace Nova
{
    void Scene::OnInit()
    {
#ifdef NOVA_HAS_PHYSICS
        PhysicsWorldCreateInfo physics2DCreateInfo;
        physics2DCreateInfo.scene = this;
        m_PhysicsWorld2D = MakeRef<PhysicsWorld2D>();
        m_PhysicsWorld2D->Initialize(physics2DCreateInfo);
#endif

#ifdef NOVA_HAS_PHYSICS3D
        m_PhysicsWorld3D.OnInit(this);
#endif
        for(Entity* entity : m_Entities)
        {
            entity->OnInit();
        }
    }

    void Scene::OnUpdate(const float deltaTime)
    {
        for(Entity* entity : m_Entities)
        {
            entity->OnUpdate(deltaTime);
        }

#ifdef NOVA_HAS_PHYSICS
        m_PhysicsWorld2D->Step();
#endif

#ifdef NOVA_HAS_PHYSICS3D
        m_PhysicsWorld3D.Step();
#endif

        for(Entity* entity : m_Entities)
        {
            entity->OnPhysicsUpdate(deltaTime);
        }
    }

    void Scene::OnPreRender(Rendering::CommandBuffer& cmdBuffer)
    {
        for(Entity* entity : m_Entities)
        {
            entity->OnPreRender(cmdBuffer);
        }
    }

    void Scene::OnRender(Rendering::CommandBuffer& cmdBuffer)
    {
        for(Entity* entity : m_Entities)
        {
            entity->OnRender(cmdBuffer);
        }
    }


    void Scene::OnDestroy()
    {
        for(const Entity* entity : m_Entities)
        {
            EntityHandle handle = EntityHandle(entity->GetUUID(), this);
            DestroyEntity(handle);
        }
#ifdef NOVA_HAS_PHYSICS
        m_PhysicsWorld2D->Destroy();
#endif

#ifdef NOVA_HAS_PHYSICS3D
        m_PhysicsWorld3D.OnDestroy();
#endif
    }

    EntityHandle Scene::CreateEntity(const String& name)
    {
        Entity* entity = m_EntityPool.New(name, this);
        entity->m_Enabled = true;
        entity->OnInit();
        m_Entities.Add(entity);
        return EntityHandle(entity->GetUUID(), this);
    }

    bool Scene::DestroyEntity(EntityHandle& handle)
    {
        if (handle == nullptr)
            return false;

        if (handle.GetContext() != this)
            return false;

        Entity* entity = handle.GetEntity();
        // Might remove this, already checked in EntityHandle::GetEntity
        if(!m_Entities.Contains(entity)) return false;

        entity->OnDestroy();
        m_Entities.Remove(entity);
        m_EntityPool.Free(entity);
        handle = nullptr;
        return true;
    }

    Application* Scene::GetOwner() const
    {
        return m_Owner;
    }


    void Scene::ForEach(const Function<void(const EntityHandle&)>& function)
    {
        for(const Entity* entity : m_Entities)
        {
            const EntityHandle Handle = EntityHandle(entity->GetUUID(), this);
            function.Call(Handle);
        }
    }

#ifdef NOVA_HAS_PHYSICS
    Ref<PhysicsWorld2D> Scene::GetPhysicsWorld2D() const
    {
        return m_PhysicsWorld2D;
    }
#endif

#ifdef NOVA_HAS_PHYSICS3D
    const PhysicsWorld3D& Scene::GetPhysicsWorld3D() const
    {
        return m_PhysicsWorld3D;
    }

    PhysicsWorld3D& Scene::GetPhysicsWorld3D()
    {
        return m_PhysicsWorld3D;
    }
#endif

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
