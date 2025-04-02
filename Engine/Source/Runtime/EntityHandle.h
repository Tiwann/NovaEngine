#pragma once
#include "UUID.h"

namespace Nova
{
    class Entity;
    
    class EntityHandle
    {
    public:
        EntityHandle(decltype(nullptr)) : m_EntityID({0, 0}) {}
        EntityHandle(const UUID& ID) : m_EntityID(ID){}
        
        operator bool() const { return IsValid(); }
        Entity* operator->() const{ return GetEntity(); }
        bool IsValid() const;
        Entity* GetEntity() const;
        
        bool operator==(const EntityHandle& Other) const;

        static EntityHandle NullHandle;
    private:
        UUID m_EntityID;
    };
}
