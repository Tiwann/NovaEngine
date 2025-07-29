﻿#pragma once
#include "Runtime/Object.h"
#include "Runtime/Uuid.h"
#include "Containers/String.h"
#include "Runtime/Flags.h"

namespace Nova
{
    class Transform;
    class Entity;
    class Scene;

    enum class ComponentFlagBits
    {
        None = 0,
        HideInInspector = BIT(0),
        DisallowMultipleInEntity = BIT(1),
        DisallowMultipleInScene = BIT(2),
    };
    typedef Flags<ComponentFlagBits> ComponentFlags;

    class Component : public Object
    {
    public:
        Component(Entity* owner, const String& name);
        Component(const Component&) = delete;
        Component(Component&&) = delete;
        Component& operator=(const Component&) = delete;
        Component& operator=(Component&&) = delete;
        ~Component() override = default;

        Transform* GetTransform() const;
        Entity* GetOwner() const;
        Scene* GetScene() const;
        
        virtual void OnInit() {}
        virtual void OnStart() {}
        virtual void OnDestroy() {}
        virtual void OnEnable() {}
        virtual void OnDisable() {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnPhysicsUpdate(float deltaTime) {}
        //virtual void OnInspectorGUI(const ImGuiIO& IO) {}

        bool IsEnabled() const;
        void SetEnabled(bool enabled);

        Uuid GetUuid() const;
        void SetFlags(ComponentFlags flags);
        ComponentFlags GetFlags() const;
    protected:
        Uuid m_Uuid = Uuid::Zero;
        bool m_Enabled = false;
        Entity* m_Entity = nullptr;
        ComponentFlags m_Flags = ComponentFlagBits::None;
    };
}
