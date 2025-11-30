#include "Entity.h"
#include "Component.h"
#include "Scene.h"
#include "Components/Transform.h"

namespace Nova
{
    Entity::Entity() : m_Uuid(Uuid::Generate())
    {

    }

    Entity::Entity(const String& name, Scene* owner) : Object(name), m_Uuid(Uuid::Generate()), m_Owner(owner)
    {
        m_Transform = AddComponent<Transform>();
    }

    bool Entity::RemoveComponent(Component* component)
    {
        if(!component) return false;
            
        component->OnDestroy();
        m_Components.Remove(component);
        delete component;
        return true;
    }

    void Entity::SetParent(Entity* entity)
    {
        if(!entity)
        {
            m_Parent->m_Children.Remove(entity);
            m_Parent = nullptr;
            return;
        }

        m_Parent = entity;
        entity->m_Children.Add(this);
    }
    
    bool Entity::HasChildren() const { return !m_Children.IsEmpty(); }

    bool Entity::HasParent() const { return m_Parent != nullptr; }

    size_t Entity::GetChildCount() const
    {
        return m_Children.Count();
    }

    Entity* Entity::GetChild(size_t index) const
    {
        if(index > m_Children.Count())
            return nullptr;
        return m_Children[index];
    }

    Entity* Entity::GetParent() const
    {
        return m_Parent;
    }

    Transform* Entity::GetTransform() const
    {
        return m_Transform;
    }

    Scene* Entity::GetOwner() 
    {
        return m_Owner;
    }

    const Scene* Entity::GetOwner() const
    {
        return m_Owner;
    }

    Entity::Iterator Entity::begin()
    { return m_Components.begin(); }

    Entity::Iterator Entity::end()
    { return m_Components.end(); }

    Entity::ConstIterator Entity::begin() const
    { return m_Components.begin(); }

    Entity::ConstIterator Entity::end() const
    { return m_Components.end(); }


    void Entity::OnInit()
    {
        ForEach([](const auto& component){ component->OnInit(); });
    }

    void Entity::OnStart()
    {
        ForEach([](const auto& component) { component->OnStart(); });
    }

    void Entity::OnUpdate(const float deltaTime)
    {
        if(!m_Enabled) return;

        for(Component* component : m_Components)
        {
            if(!component->IsEnabled())
                continue;
            component->OnUpdate(deltaTime);
        }
    }

    void Entity::OnPhysicsUpdate(const float deltaTime)
    {
        if(!m_Enabled) return;
        for(Component* Component : m_Components)
        {
            if(!Component->IsEnabled())
                continue;
            Component->OnPhysicsUpdate(deltaTime);
        }
    }

    void Entity::OnPreRender(CommandBuffer& cmdBuffer)
    {
        if(!m_Enabled) return;

        for(Component* component : m_Components)
        {
            if(!component->IsEnabled())
                continue;
            component->OnPreRender(cmdBuffer);
        }
    }

    void Entity::OnRender(CommandBuffer& cmdBuffer)
    {
        if(!m_Enabled) return;

        for(Component* component : m_Components)
        {
            if(!component->IsEnabled())
                continue;
            component->OnRender(cmdBuffer);
        }
    }

    void Entity::OnDrawDebug()
    {
        if(!m_Enabled) return;

        for(Component* component : m_Components)
        {
            if(!component->IsEnabled())
                continue;
            component->OnDrawDebug();
        }
    }

    void Entity::OnDestroy()
    {
        for(Component* component : m_Components)
        {
            component->OnDestroy();
            delete component;
        }
        m_Components.Clear();
    }

    bool Entity::IsEnabled() const
    {
        return m_Enabled;
    }

    void Entity::SetEnabled(const bool enabled)
    {
        if(m_Enabled == enabled)
            return;
        
        m_Enabled = enabled;
        for(Component* Component : m_Components)
        {
            if(m_Enabled)
                Component->OnEnable();
            else
                Component->OnDisable();
        }
    }

    void Entity::ToggleEnabled()
    {
        if(m_Enabled) SetEnabled(false);
        else SetEnabled(true);
    }

    EntityHideFlags Entity::GetHideFlags() const
    {
        return m_HideFlags;
    }

    void Entity::SetHideFlags(const EntityHideFlags hideFlags)
    {
        m_HideFlags = hideFlags;
    }
}
