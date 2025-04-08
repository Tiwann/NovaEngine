#include "LightComponent.h"
#include "Editor/EditorGUI.h"

namespace Nova
{
    LightComponent::LightComponent(Entity* Owner, const String& Name = "Light Component") : Component(Owner, Name)
    {
        
    }

    void LightComponent::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);
        const StaticArray<const char*, 3> LightTypes
        {
            "Directional",
            "Point",
            "Spot"
        };
        ImGui::BeginDisabled();
        ImGui::Combo("Light Type", (int*)&m_Type, LightTypes.Data(), (i32)LightTypes.Count());
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

    Color LightComponent::GetColor() const
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
