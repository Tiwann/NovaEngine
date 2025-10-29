#include "LightComponent.h"
#include "Containers/StaticArray.h"

namespace Nova
{
    LightComponent::LightComponent(Entity* owner, const String& name = "Light Component") : Component(owner, name)
    {
        
    }

    /*void LightComponent::OnInspectorGUI(const ImGuiIO& IO)
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
        ImGui::ColorEdit3("Color", (float*)&m_Color);
    }*/

    float LightComponent::GetIntensity() const
    {
        return m_Intensity;
    }

    void LightComponent::SetIntensity(const float intensity)
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
