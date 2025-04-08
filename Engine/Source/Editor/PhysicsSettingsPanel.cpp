#include "PhysicsSettingsPanel.h"
#include "EditorGUI.h"

namespace Nova
{
    void PhysicsSettingsPanel::OnInspectorGUI(const ImGuiIO& IO)
    {
        EditorWindow::OnInspectorGUI(IO);
        if(!m_Opened) return;


        UI::NewWindow(m_Name, m_Opened, WindowFlagBits::None, []
        {
            ImGui::SeparatorText("Physics2D");
            UI::DragVector2<f32>("Base Gravity", Physics2D::Gravity, 0.01f);
            UI::DragValue<f32>("Timestep", Physics2D::TimeStep, 0.01f);

            ImGui::SeparatorText("Physics3D");
            UI::DragVector3<f32>("Base Gravity", Physics3D::Gravity, 0.01f);
            UI::DragValue<f32>("Timestep", Physics3D::TimeStep, 0.01f);
        });
        
    }
}
