#include "LightComponent.h"
#include "Editor/EditorGUI.h"
#include "Containers/StaticArray.h"

namespace Nova
{
    LightComponent::LightComponent(Entity* Owner, const String& Name = "Light Component") : Component(Owner, Name)
    {
        
    }

    void LightComponent::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);
        const char* LightTypes[4]
        {
            "Directional",
            "Point",
            "Spot",
            "Ambient"
        };
        ImGui::BeginDisabled();
        ImGui::Combo("Light Type", (int*)&m_Type, LightTypes, std::size(LightTypes));
        ImGui::EndDisabled();
        ImGui::Separator();
        UI::DragValue("Intensity", m_Intensity, 0.1f, 0.0f, 10.0f);
        ImGui::ColorEdit3("Color", (f32*)&m_Color);
    }

    f32 LightComponent::GetIntensity() const
    {
        return m_Intensity;
    }

    void LightComponent::SetIntensity(const f32 intensity)
    {
        m_Intensity = intensity;
    }

    const Color& LightComponent::GetColor() const
    {
        return m_Color;
    }

    void LightComponent::SetColor(const Color& color)
    {
        m_Color = color;
    }

    LightType LightComponent::GetType() const
    {
        return m_Type;
    }
}
