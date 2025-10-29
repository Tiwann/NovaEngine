#include "EntityHandle.h"
#include "Scene.h"
#include "Entity.h"

namespace Nova
{

    bool EntityHandle::IsValid() const
    {
        if (m_EntityID == Uuid::Zero || m_Context == nullptr)
            return false;

        const Array<Entity*> entities = m_Context->GetEntities();
        const Array<Uuid> entitiesUuids = entities.Transform<Uuid>([](const Entity* entity) { return entity->GetUUID(); });
        return entitiesUuids.Contains(m_EntityID);
    }

    Entity* EntityHandle::GetEntity() const
    {
        if (*this == nullptr) return nullptr;
        const Array<Entity*> entities = m_Context->GetEntities();
        Entity** result = entities.Single([this](const Entity* entity)
        {
            return m_EntityID == entity->GetUUID();
        });
        return result ? *result : nullptr;
    }

    Scene* EntityHandle::GetContext() const
    {
        return m_Context;
    }

    bool EntityHandle::operator==(const EntityHandle& Other) const
    {
        return m_EntityID == Other.m_EntityID;
    }
}
