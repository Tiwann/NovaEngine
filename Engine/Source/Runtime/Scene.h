#pragma once
#include "Object.h"
#include "UUID.h"
#include "Entity.h"
#include "Ref.h"
#include "Containers/Function.h"
#include "Containers/String.h"
#include "Containers/BumpAllocator.h"

#ifdef NOVA_HAS_PHYSICS3D
#include "Physics/PhysicsWorld3D.h"
#endif

namespace Nova { class CommandBuffer; }

namespace Nova
{
    class Application;
    class PhysicsWorld2D;

    class Scene : public Object
    {
    public:
        using Iterator = Array<Entity*>::Iterator;
        using ConstIterator = Array<Entity*>::ConstIterator;

        Scene(Application* owner, const String& name) : Object(name), m_Owner(owner) {}
        ~Scene() override = default;
         
        void OnInit();
        void OnUpdate(float deltaTime);
        void OnPreRender(CommandBuffer& cmdBuffer);
        void OnRender(CommandBuffer& cmdBuffer);
        void OnDrawDebug();
        void OnDestroy();

        template<typename ComponentType>
        ComponentType* GetFirstComponent()
        {
            for (const Entity* entity : m_Entities)
            {
                ComponentType* component = entity->GetComponent<ComponentType>();
                if (!component) continue;
                return component;
            }
            return nullptr;
        }

        template<typename ComponentType>
        const ComponentType* GetFirstComponent() const
        {
            for (const Entity* entity : m_Entities)
            {
                ComponentType* component = entity->GetComponent<ComponentType>();
                if (!component) continue;
                return component;
            }
            return nullptr;
        }

        template <typename ComponentType>
        Array<ComponentType*> GetAllComponents()
        {
            Array<ComponentType*> result;
            ForEach([&result](const EntityHandle& handle)
            {
                if (Array<ComponentType*> components = handle->GetAllComponents<ComponentType>(); !components.IsEmpty())
                    result.AddRange(components);
            });
            return result;
        }
        
        EntityHandle CreateEntity(const String& name);
        bool DestroyEntity(EntityHandle& handle);

        UUID GetGuid() const { return m_Uuid; }
        void ForEach(const Function<void(const EntityHandle&)>& function);

        Application* GetOwner() const;


#ifdef NOVA_HAS_PHYSICS
        Ref<PhysicsWorld2D> GetPhysicsWorld2D() const;
#endif
#ifdef NOVA_HAS_PHYSICS3D
        const PhysicsWorld3D& GetPhysicsWorld3D() const;
        PhysicsWorld3D& GetPhysicsWorld3D();
#endif
        Iterator begin();
        Iterator end();
        ConstIterator begin() const;
        ConstIterator end() const;

        Array<Entity*> GetEntities();

    private:
        UUID m_Uuid;
        Array<Entity*> m_Entities;
        Application* m_Owner = nullptr;
#ifdef NOVA_HAS_PHYSICS
        Ref<PhysicsWorld2D> m_PhysicsWorld2D = nullptr;
#endif
#ifdef NOVA_HAS_PHYSICS3D
        PhysicsWorld3D m_PhysicsWorld3D;
#endif
    };
}
