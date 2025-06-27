#include "Runtime/Component.h"

#include "Entity.h"

namespace Nova
{
    Component::Component(Entity* Owner, String Name) : Object(Name), m_Name(std::move(Name)), m_Enabled(true),
                                                       m_Entity(Owner)
    {
    }

    void Component::OnInit()
    {
    }

    void Component::OnStart()
    {
    }

    void Component::OnDestroy()
    {
        m_Enabled = false;
    }

    void Component::OnEnable()
    {
    }

    void Component::OnDisable()
    {
    }

    void Component::OnUpdate(f32 Delta)
    {
    }

    void Component::OnFrameBegin(Renderer* Renderer)
    {
    }

    void Component::OnPhysicsUpdate(f32 Delta)
    {
    }

    void Component::OnRender(Renderer* Renderer)
    {
    }

    void Component::OnInspectorGUI(const ImGuiIO& IO)
    {
        ImGui::Checkbox("Enabled", &m_Enabled);
    }

    bool Component::IsEnabled() const
    {
        return m_Enabled;
    }


    const String& Component::GetName() const
    {
        return m_Name;
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
        return m_Entity->GetScene();
    }

    void Component::SetEnabled(bool Enabled)
    {
        if (m_Enabled == Enabled)
            return;
        m_Enabled = Enabled;
    }

    UUID Component::GetGuid() const
    {
        return m_UUID;
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
