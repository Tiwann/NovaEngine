#include "Component.h"
#include "Entity.h"
#include "Scene.h"

namespace Nova
{
    Component::Component(Entity* owner, const String& name) : Object(name), m_Enabled(true), m_Entity(owner), m_Uuid(Uuid::Generate())
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

    Application* Component::GetApplication() const
    {
        const Scene* scene = GetScene();
        return scene->GetOwner();
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

    void Component::SetComponentFlags(const ComponentFlags flags)
    {
        m_ComponentFlags = flags;
    }

    ComponentFlags Component::GetComponentFlags() const
    {
        return m_ComponentFlags;
    }
}
