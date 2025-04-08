#include "Selection.h"

namespace Nova
{
    EntityHandle Selection::m_Entity = EntityHandle::NullHandle;
    
    void Nova::Selection::SetEntity(const EntityHandle& Entity)
    {
        m_Entity = Entity;
    }

    Nova::EntityHandle Nova::Selection::GetEntity()
    {
        return m_Entity;
    }

    void Nova::Selection::SetComponent(Component* Component)
    {
        m_Component = Component;
    }

    Nova::Component* Nova::Selection::GetComponent()
    {
        return m_Component;
    }
}
