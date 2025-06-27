#include "Entity.h"
#include "Application.h"
#include "Scene.h"
#include "Components/Transform.h"
#include "Editor/Selection.h"

namespace Nova
{
    EntityHandle Entity::NullHandle = EntityHandle::NullHandle;
    
    Entity::Entity() : m_UUID(UUID::Zero), m_Name("Invalid Entity"), m_Enabled(false)
    {
        
    }

    Entity::Entity(String Name, Scene* Owner) : Object(Name), m_Name(std::move(Name)), m_Scene(Owner)
    {
        m_Transform = AddComponent<Transform>();
    }

    void Entity::SetName(const String& Name)
    {
        m_Name = Name;
    }

    bool Entity::RemoveComponent(Component* Component)
    {
        if(!Component) return false;
            
        Component->OnDestroy();
        m_Components.Remove(Component);
        delete Component;
        return true;
    }

    void Entity::SetParent(Entity* Entity)
    {
        if(!Entity)
        {
            m_Parent = nullptr;
            Entity->m_Children.Remove(Entity);
            return;
        }

        m_Parent = Entity;
        Entity->m_Children.Add(this);
    }
    
    bool Entity::HasChildren() const { return !m_Children.IsEmpty(); }

    bool Entity::HasParent() const { return m_Parent != nullptr; }

    Entity* Entity::GetChild(size_t Index) const
    {
        if(Index < 0 || Index > m_Children.Count())
            return nullptr;
        return m_Children[Index];
    }

    Entity* Entity::GetParent() const
    {
        return m_Parent;
    }

    Transform* Entity::GetTransform() const
    {
        return m_Transform;
    }

    Scene* Entity::GetScene() 
    {
        return m_Scene;
    }

    const Scene* Entity::GetScene() const
    {
        return m_Scene;
    }


    void Entity::OnInit()
    {
        ForEach([](const auto& Component){ Component->OnInit(); });
    }

    void Entity::OnStart()
    {
        ForEach([](const auto& Component) { Component->OnStart(); });
    }

    void Entity::OnUpdate(f32 Delta)
    {
        if(!m_Enabled) return;
        for(Component* Component : m_Components)
        {
            if(!Component->m_Enabled)
                continue;
            Component->OnUpdate(Delta);
        }
    }

    void Entity::OnFrameBegin(Renderer* Renderer)
    {
        if (!m_Enabled)
            return;

        for(Component* Component : m_Components)
        {
            if(!Component->m_Enabled)
                continue;

            Component->OnFrameBegin(Renderer);
        }
    }

    void Entity::OnRender(Renderer* Renderer)
    {
        if(!m_Enabled) return;
        for(Component* Component : m_Components)
        {
            if(!Component->m_Enabled)
                continue;
            
            Component->OnRender(Renderer);
        }
    }

    void Entity::OnPhysicsUpdate(f32 Delta)
    {
        if(!m_Enabled) return;
        for(Component* Component : m_Components)
        {
            if(!Component->m_Enabled)
                continue;
            Component->OnPhysicsUpdate(Delta);
        }
    }

    void Entity::OnDestroy()
    {
        for(ComponentArray::SizeType Index = 0; Index < m_Components.Count(); Index++)
        {
            m_Components[Index]->OnDestroy();
            delete m_Components[Index];
        }
        m_Components.Clear();
    }

    

    bool Entity::IsEnabled() const
    {
        return m_Enabled;
    }

    void Entity::SetEnabled(bool Enabled)
    {
        if(m_Enabled == Enabled)
            return;
        
        m_Enabled = Enabled;
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

    void Entity::SetHideFlags(EntityHideFlags HideFlags)
    {
        m_HideFlags = HideFlags;
    }
}
