#pragma once
#include "EntityHandle.h"
#include "Game/Component.h"
#include "Containers/Function.h"
#include "Containers/String.h"
#include "Containers/StringFormat.h"
#include "Runtime/Uuid.h"
#include "Runtime/Flags.h"
#include "Runtime/Object.h"

namespace Nova::Rendering { class CommandBuffer; }

namespace Nova
{
    class Component;
    class Transform;

    enum class EntityHideFlagBits
    {
        None = 0,
        HideInHierarchy = BIT(0),
        HideInInspector = BIT(1),
        HideInScene = BIT(2),
    };

    typedef Flags<EntityHideFlagBits> EntityHideFlags;
    
    
    class Entity : public Object
    {
    public:
        using Iterator = Array<Component*>::Iterator;
        using ConstIterator = Array<Component*>::ConstIterator;

        Entity();
        Entity(const String& name, Scene* owner);
        ~Entity() override = default;
        
        template<typename T> requires std::is_base_of_v<Component, T>
        T* GetComponent() const
        {
            for(Component* component : m_Components)
            {
                if(T* casted = dynamic_cast<T*>(component))
                    return casted;
            }
            return nullptr;
        }

        template<typename T> requires std::is_base_of_v<Component, T>
        Array<T*> GetAllComponents() const
        {
            Array<T*> result;
            for(Component* component : m_Components)
            {
                if (!component) continue;
                if(T* casted = dynamic_cast<T*>(component))
                    result.Add(casted);
            }
            return result;
        }

        template<typename T> requires std::is_base_of_v<Component, T>
        T* GetComponentInChildren() const
        {
            for(const Entity* child : m_Children)
            {
                return child->GetComponent<T>();
            }
            return nullptr;
        }
        
        template<typename T> requires std::is_base_of_v<Component, T>
        T* AddComponent()
        {
            T* newComponent = new T(this);
            m_Components.Add(newComponent);
            newComponent->OnInit();
            /*NOVA_LOG(Entity, Verbosity::Trace, "Created {} component on Entity {}", NewComponent->GetName(), m_Name);*/
            return newComponent;
        }
        
        template<typename T> requires std::is_base_of_v<Component, T>
        bool RemoveComponent()
        {
            for(Component* component : m_Components)
            {
                if(T* casted = dynamic_cast<T*>(component))
                {
                    casted->OnDestroy();
                    m_Components.Remove(component);
                    delete component;
                    return true;
                }
            }
            return false;
        }

        bool RemoveComponent(Component* component);
        void SetParent(Entity* entity);
        bool HasChildren() const;
        bool HasParent() const;
        
        Entity* GetChild(size_t index) const;
        Entity* GetParent() const;
        Transform* GetTransform() const;

        Scene* GetOwner();
        const Scene* GetOwner() const;

        Iterator begin();
        Iterator end();
        ConstIterator begin() const;
        ConstIterator end() const;

        void ForEach(const Function<void(Component*)>& function) const
        {
            for(Component* component : m_Components)
                function(component);
        }

        Uuid GetUUID() const { return m_Uuid; }
    
        bool IsEnabled() const;
        void SetEnabled(bool enabled);
        void ToggleEnabled();
        EntityHideFlags GetHideFlags() const;
        void SetHideFlags(EntityHideFlags hideFlags);
    protected:
        virtual void OnInit();
        virtual void OnStart();
        virtual void OnUpdate(float deltaTime);
        virtual void OnPhysicsUpdate(float deltaTime);
        virtual void OnRender(Rendering::CommandBuffer& cmdBuffer);
        virtual void OnDestroy();

    private:
        friend class Scene;
        Uuid m_Uuid = Uuid::Zero;
        Array<Component*> m_Components;
        bool m_Enabled = false;
        Array<Entity*> m_Children;
        Entity* m_Parent = nullptr;
        Transform* m_Transform = nullptr;
        Scene* m_Owner = nullptr;
        EntityHideFlags m_HideFlags = EntityHideFlagBits::None;
    };

    
}
