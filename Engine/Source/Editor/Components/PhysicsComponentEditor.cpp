#include "PhysicsComponentEditor.h"
#include "External/ImGuiExtension.h"

namespace Nova
{
    void PhysicsComponentEditor::OnGui()
    {
        ImGui::PushID(m_Target);


        ImGui::End();
    }
}
