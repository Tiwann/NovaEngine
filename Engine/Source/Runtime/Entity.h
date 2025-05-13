#pragma once
#include "EntityHandle.h"
#include "Runtime/Component.h"
#include "UUID.h"
#include "Log.h"
#include "LogCategory.h"
#include "LogVerbosity.h"
#include "Containers/Function.h"
#include "Containers/String.h"
#include "Containers/StringFormat.h"
#include "Runtime/Flags.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(Entity, "Entity")

namespace Nova
{
    class Component;
    class Transform;
    class Renderer;
    struct Vector3;

    enum class EntityHideFlagBits
    {
        None = 0,
        NOVA_FLAG_BIT(HideInHierarachy, 0),
        NOVA_FLAG_BIT(HideInInspector, 1),
        NOVA_FLAG_BIT(HideInScene, 2),
        Internal = HideInHierarachy | HideInInspector | HideInScene
    };

    NOVA_DECLARE_FLAGS(EntityHideFlagBits, EntityHideFlags)
    
    
    class Entity
    {
    public:
        using ComponentArray = Array<Component*>;
        using Iterator = ComponentArray::Iterator;
        using ConstIterator = ComponentArray::ConstIterator;
        
        friend class Scene;
        friend class Application;
        friend class DetailsPanel;
        Entity();
        Entity(String Name, Scene* Owner);
        virtual ~Entity() = default;
        
        template<typename T> requires IsBaseOfValue<Component, T>
        T* GetComponent() const
        {
            for(Component* Component : m_Components)
            {
                if(T* CastedComponent = dynamic_cast<T*>(Component))
                    return CastedComponent;
            }
            return nullptr;
        }

        template<typename T> requires IsBaseOfValue<Component, T>
        Array<T*> GetAllComponents() const
        {
            Array<T*> Result;
            for(Component* Component : m_Components)
            {
                if(T* CastedComponent = dynamic_cast<T*>(Component))
                    Result.Add(CastedComponent);
            }
            return Result;
        }

        template<typename T> requires IsBaseOfValue<Component, T>
        T* GetComponentInChildren() const
        {
            for(const Entity* Child : m_Children)
            {
                return Child->GetComponent<T>();
            }
            return nullptr;
        }
        
        template<typename T> requires IsBaseOfValue<Component, T>
        T* AddComponent()
        {
            T* NewComponent = new T(this);
            m_Components.Add(NewComponent);
            NewComponent->OnInit();
            NOVA_LOG(Entity, Verbosity::Trace, "Created {} component on Entity {}", NewComponent->GetName(), m_Name);
            return NewComponent;
        }
        
        template<typename T> requires IsBaseOfValue<Component, T>
        bool RemoveComponent()
        {
            for(Component* Component : m_Components)
            {
                if(T* CastedComponent = dynamic_cast<T*>(Component))
                {
                    CastedComponent->OnDestroy();
                    m_Components.Remove(Component);
                    delete Component;
                    return true;
                }
            }
            return false;
        }

        bool RemoveComponent(Component* Component);
        void SetParent(Entity* Entity);
        bool HasChildren() const;
        bool HasParent() const;
        
        Entity* GetChild(size_t Index) const;
        Entity* GetParent() const;
        Transform* GetTransform() const;

        Scene* GetScene();
        const Scene* GetScene() const;

        void SetName(const String& Name);
        const String& GetName() const { return m_Name; }
        
        Iterator begin() { return m_Components.begin(); }
        Iterator end() { return m_Components.end(); }
        ConstIterator begin() const { return m_Components.begin(); }
        ConstIterator end() const { return m_Components.end(); }

        void ForEach(const Function<void(Component*)>& Delegate) const
        {
            for(Component* Component : m_Components)
                Delegate(Component);
        }

        UUID GetGuid() const { return m_UUID; }
    
        bool IsEnabled() const;
        void SetEnabled(bool Enabled);
        void ToggleEnabled();
        EntityHideFlags GetHideFlags() const;
        void SetHideFlags(EntityHideFlags HideFlags);

        static EntityHandle NullHandle;
    protected:
        virtual void OnInit();
        virtual void OnStart();
        virtual void OnUpdate(f32 Delta);
        virtual void OnFrameBegin(Renderer* Renderer);
        virtual void OnRender(Renderer* Renderer);
        virtual void OnPhysicsUpdate(f32 Delta);
        virtual void OnDestroy();
        

        UUID m_UUID;
        String m_Name;
        Array<Component*> m_Components;
        bool m_Enabled;
        Array<Entity*> m_Children;
        Entity* m_Parent = nullptr;
        Transform* m_Transform = nullptr;
        Scene* m_Scene = nullptr;
        EntityHideFlags m_HideFlags;
    };

    
}
