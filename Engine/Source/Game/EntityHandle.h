#pragma once
#include "Runtime/Uuid.h"

namespace Nova
{
    class Entity;
    class Scene;
    
    class EntityHandle
    {
    public:
        EntityHandle(decltype(nullptr)) : m_EntityID(Uuid(0, 0)), m_Context(nullptr) {}
        EntityHandle(const Uuid& id, Scene* context) : m_EntityID(id), m_Context(context) {}
        EntityHandle(const EntityHandle& other) : m_EntityID(other.m_EntityID), m_Context(other.m_Context) {}
        EntityHandle(EntityHandle&& other) noexcept
        {
            if (this == &other)
                return;

            m_EntityID = other.m_EntityID;
            m_Context = other.m_Context;
            other.m_EntityID = Uuid::Zero;
            other.m_Context = nullptr;
        }

        EntityHandle& operator=(const EntityHandle& other)
        {
            m_EntityID = other.m_EntityID;
            m_Context = other.m_Context;
            return *this;
        }

        EntityHandle& operator=(EntityHandle&& other) noexcept
        {
            if (this == &other)
                return *this;
            m_EntityID = other.m_EntityID;
            m_Context = other.m_Context;
            other.m_EntityID = Uuid::Zero;
            other.m_Context = nullptr;
            return *this;
        }


        bool IsValid() const;
        Entity* GetEntity() const;

        operator bool() const { return IsValid(); }
        Entity* operator->() const{ return GetEntity(); }

        bool operator==(const EntityHandle& other) const;
    private:
        Uuid m_EntityID;
        Scene* m_Context = nullptr;
    };
}
