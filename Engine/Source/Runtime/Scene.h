#pragma once
#include "Containers/Buffer.h"
#include "LogCategory.h"
#include "UUID.h"
#include "Entity.h"
#include "Containers/Function.h"
#include "Containers/String.h"

#include "Core/Physics/PhysicsWorld2D.h"
#include "Core/Physics/PhysicsWorld3D.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(Scene, "SCENE");

namespace Nova
{
    class Renderer;
    struct Vector3;
    
    class Scene final
    {
    public:
        using Iterator = Array<Entity*>::Iterator;
        using ConstIterator = Array<Entity*>::ConstIterator;
        friend class Physics2D;

        Scene() = default;
        ~Scene() = default;
         
        void OnInit();
        void OnUpdate(f32 Delta);
        void OnRender(Renderer* Renderer) const;
        void OnDestroy();

        template <typename Component>
        Array<Component*> GetAllComponents()
        {
            Array<Component*> Result;
            ForEach([&Result](const EntityHandle& Handle)
            {
                if (Array<Component*> Components = Handle->GetAllComponents<Component>(); !Components.IsEmpty())
                    Result.AddRange(Components);
            });
            return Result;
        }
        
        EntityHandle CreateEntity(const String& Name);
        bool DestroyEntity(EntityHandle& Handle);
        
        GUID GetGuid() const { return m_Guid; }
        const String& GetName() const { return m_Name; }
        void SetName(const String& Name);

        void ForEach(const Function<void(const EntityHandle&)>& Delegate);
        
        const PhysicsWorld2D& GetPhysicsWorld2D() const;
        const PhysicsWorld3D& GetPhysicsWorld3D() const;
        PhysicsWorld2D& GetPhysicsWorld2D();
        PhysicsWorld3D& GetPhysicsWorld3D();
        
        Iterator begin();
        Iterator end();
        ConstIterator begin() const;
        ConstIterator end() const;

        Array<Entity*> GetEntities();
    private:
        GUID m_Guid;
        String m_Name;
        BumpAllocator<Entity, 8192> m_EntityPool;
        Array<Entity*> m_Entities;
        PhysicsWorld2D m_PhysicsWorld2D;
        PhysicsWorld3D m_PhysicsWorld3D;
    };
}
