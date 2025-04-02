#pragma once
#include "Runtime/UUID.h"
#include "Platform/PlatformImGui.h"
#include "Containers/String.h"
#include "Runtime/Flags.h"
#include <CoreExport.h>


namespace Nova
{
    class Transform;
    class Entity;
    class Renderer;
    class Scene;

    enum class ComponentFlagBits
    {
        None = 0,
        NOVA_FLAG_BIT(HideInInspector, 0),
        NOVA_FLAG_BIT(DisallowMultipleInEntity, 1),
        NOVA_FLAG_BIT(DisallowMultipleInScene, 2),
    };

    NOVA_DECLARE_FLAGS(ComponentFlagBits, ComponentFlags)

    class CORE_API Component
    {
    public:
        Component(Entity* Owner, String Name);
        Component(const Component&) = delete;
        Component(Component&&) = delete;
        Component& operator=(const Component&) = delete;
        Component& operator=(Component&&) = delete;
        virtual ~Component();

        const String& GetName() const;
        Transform* GetTransform() const;
        Entity* GetOwner() const;
        Scene* GetScene() const;
        
        virtual void OnInit();
        virtual void OnStart();

        virtual void OnDestroy();
        virtual void OnEnable();
        virtual void OnDisable();
        virtual void OnUpdate(f32 Delta);
        virtual void OnPhysicsUpdate(f32 Delta);
        virtual void OnRender(Renderer* Renderer);
        virtual void OnInspectorGUI(const ImGuiIO& IO);

        bool IsEnabled() const;
        void SetEnabled(bool Enabled);

        UUID GetGuid() const;
        void SetFlags(ComponentFlags Flags);
        ComponentFlags GetFlags() const;
    protected:
        UUID m_UUID;
        String m_Name;
        bool m_Enabled;
        Entity* m_Entity{nullptr};
        ComponentFlags m_Flags;
    private:
        friend class Entity;
    };
}
