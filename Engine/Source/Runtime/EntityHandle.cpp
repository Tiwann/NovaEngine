#include "EntityHandle.h"
#include "Scene.h"
#include "Application.h"
#include "Entity.h"

namespace Nova
{
    EntityHandle EntityHandle::NullHandle = EntityHandle({0, 0});
    
    bool EntityHandle::IsValid() const
    {
        if (*this == EntityHandle::NullHandle) return false;
        
        Scene* Scene = g_Application->GetScene();
        if (!Scene) return false;
        const Array<Entity*> Entities = Scene->GetEntities();
        
        Array<UUID> EntitiesGuids;
        for (const Entity* Entity : Entities)
            EntitiesGuids.Add(Entity->GetGuid());
        
        for (const UUID& ID : EntitiesGuids)
        {
            if (ID == m_EntityID)
                return true;
        }
        return false;
    }

    Entity* EntityHandle::GetEntity() const
    {
        if (*this == NullHandle) return nullptr;
        Scene* Scene = g_Application->GetScene();
        if (!Scene) return nullptr;
        
        const Array<Entity*> Entities = Scene->GetEntities();
        Entity** Result = Entities.Single([this](const Entity* Entity)
        {
            return m_EntityID == Entity->GetGuid();
        });
        return Result ? *Result : nullptr;
    }
    
    bool ::Nova::EntityHandle::operator==(const EntityHandle& Other) const
    {
        return m_EntityID == Other.m_EntityID;
    }
}
