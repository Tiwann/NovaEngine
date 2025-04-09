#include "DetailsPanel.h"
#include "EditorGUI.h"
#include "Selection.h"
#include "Runtime/Scene.h"
#include "Containers/StringFormat.h"

namespace Nova
{
    DetailsPanel::DetailsPanel(Scene* InScene) : EditorWindow("Details"), m_CurrentScene(InScene)
    {
        m_Opened = true;
    }
    
    void DetailsPanel::OnInspectorGUI(const ImGuiIO& IO)
    {
        EditorWindow::OnInspectorGUI(IO);
        UI::NewWindow(m_Name, m_Opened, WindowFlagBits::None, [&IO]
        {
            EntityHandle SelectedEntityHandle = Selection::GetEntity();
            if (!SelectedEntityHandle.IsValid()) return;

            Entity* SelectedEntity = SelectedEntityHandle.GetEntity();
            if (!SelectedEntity) return;
            if (SelectedEntityHandle->GetHideFlags().Contains(EntityHideFlagBits::HideInInspector))
                return;

            char Buffer[256] = {};
            std::copy(SelectedEntity->GetName().begin(), SelectedEntity->GetName().end(), Buffer);

            ImGui::PushID(SelectedEntity->GetGuid());
            ImGui::PushItemWidth(200);
            if (ImGui::InputText("##", Buffer, 256, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                SelectedEntity->SetName({Buffer});
            }
            ImGui::PopItemWidth();
            
            ImGui::SameLine();
            UI::AddComponent(SelectedEntity);
            ImGui::SameLine();
            ImGui::Checkbox("Enabled", &SelectedEntity->m_Enabled);
            ImGui::PopID();

            UI::Text(Format("UUID: {}", SelectedEntity->GetGuid().GetString()));
            static bool ShowContextMenu;
            SelectedEntity->ForEach([&IO](Component* Component)
            {
                if (Component->GetFlags().Contains(ComponentFlagBits::HideInInspector))
                    return;
                
                ImGui::PushID(Component->GetGuid());
                if (ImGui::TreeNode(*Component->GetName()))
                {
                    Component->OnInspectorGUI(IO);
                    ImGui::TreePop();
                }
                ImGui::PopID();

                if(UI::ItemClicked(MouseButton::Right))
                {
                    ShowContextMenu = true;
                    ImGui::OpenPopup("ComponentContextMenu");
                    Selection::SetComponent(Component);
                }
                
            });


            if (ShowContextMenu && ImGui::BeginPopup("ComponentContextMenu"))
            {
                const char* Options[]
                {
                    "Delete"
                };

                ImGui::SeparatorText("Context Menu");
                for (size_t i = 0; i < std::size(Options); i++)
                {
                    if (ImGui::Selectable(Options[i]))
                    {
                        Component* SelectedComponent = Selection::GetComponent();
                        if(!SelectedEntity) break;
                            
                        switch (i)
                        {
                        case 0: SelectedEntity->RemoveComponent(SelectedComponent);
                        }
                        ShowContextMenu = false;
                    }
                }

                ImGui::EndPopup();
            }
        });
    }
}
