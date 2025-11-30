#include "LightComponent.h"
#include "Containers/StaticArray.h"
#include <imgui.h>

namespace Nova
{
    LightComponent::LightComponent(Entity* owner, const String& name = "Light Component") : Component(owner, name)
    {
        
    }

    void LightComponent::OnGui()
    {
        Component::OnGui();
        const char* lightTypes[4]
        {
            "Directional",
            "Point",
            "Spot",
            "Ambient"
        };

        ImGui::BeginDisabled();
        ImGui::Combo("Light Type", (int*)&m_Type, lightTypes, std::size(lightTypes));
        ImGui::EndDisabled();
        ImGui::Separator();
        ImGui::DragFloat("Intensity", &m_Intensity, 0.1f, 0.0f, 20.0f);
        ImGui::ColorEdit3("Color", (float*)&m_Color);
    }

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
