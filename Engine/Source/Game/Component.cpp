#include "Component.h"
#include "Entity.h"

namespace Nova
{
    Component::Component(Entity* owner, const String& name) : Object(name), m_Enabled(true), m_Entity(owner)
    {
    }


    bool Component::IsEnabled() const
    {
        return m_Enabled;
    }

    Transform* Component::GetTransform() const
    {
        return m_Entity->GetTransform();
    }

    Entity* Component::GetOwner() const
    {
        return m_Entity;
    }

    Scene* Component::GetScene() const
    {
        return m_Entity->GetOwner();
    }

    void Component::SetEnabled(bool Enabled)
    {
        if (m_Enabled == Enabled)
            return;
        m_Enabled = Enabled;
    }

    Uuid Component::GetUuid() const
    {
        return m_Uuid;
    }

    void Component::SetFlags(ComponentFlags Flags)
    {
        m_Flags = Flags;
    }

    ComponentFlags Component::GetFlags() const
    {
        return m_Flags;
    }
}
