#include "PhysicsComponentEditor.h"
#include "External/ImGuiExtension.h"

namespace Nova
{
    void PhysicsComponentEditor::OnInspectorGui()
    {
        ImGui::PushID(m_Target);


        ImGui::End();
    }
}
