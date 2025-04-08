#pragma once
#include "Runtime/Entity.h"
#include "Containers/Array.h"

namespace Nova
{
    class Entity;
    class Component;
}

namespace Nova
{
    class Selection
    {
    public:
        static void SetEntity(const EntityHandle& Entity);
        static EntityHandle GetEntity();

        static void SetComponent(Component* Component);

        static Component* GetComponent();

    private:
        static EntityHandle m_Entity;
        static inline Component* m_Component;
        static inline Array<Entity*> m_Entities;
    };
}
