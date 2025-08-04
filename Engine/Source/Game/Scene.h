#pragma once
#include "Runtime/Object.h"
#include "Runtime/Uuid.h"
#include "Entity.h"
#include "Containers/Function.h"
#include "Containers/String.h"
#include "Containers/BumpAllocator.h"

#ifdef NOVA_HAS_PHYSICS
#include "Physics/PhysicsWorld2D.h"
#endif

#ifdef NOVA_HAS_PHYSICS3D
#include "Physics/PhysicsWorld3D.h"
#endif

namespace Nova
{
    class Scene : public Object
    {
    public:
        using Iterator = Array<Entity*>::Iterator;
        using ConstIterator = Array<Entity*>::ConstIterator;

        Scene(const String& name) : Object(name) {}
        ~Scene() override = default;
         
        void OnInit();
        void OnUpdate(float deltaTime);
        void OnDestroy();

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
        
        Uuid GetGuid() const { return m_Uuid; }
        const String& GetName() const { return m_Name; }
        void SetName(const String& Name);

        void ForEach(const Function<void(const EntityHandle&)>& function);


#ifdef NOVA_HAS_PHYSICS
        const PhysicsWorld2D& GetPhysicsWorld2D() const;
        PhysicsWorld2D& GetPhysicsWorld2D();
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
        Uuid m_Uuid;
        String m_Name;
        BumpAllocator<Entity, 32> m_EntityPool;
        Array<Entity*> m_Entities;
#ifdef NOVA_HAS_PHYSICS
        PhysicsWorld2D m_PhysicsWorld2D;
#endif
#ifdef NOVA_HAS_PHYSICS3D
        PhysicsWorld3D m_PhysicsWorld3D;
#endif
    };
}
