#include "InspectorWindow.h"
#include "Editor.h"
#include "Selection.h"
#include "Containers/StringFormat.h"
#include "External/ImGuiExtension.h"
#include "Input/MouseButton.h"
#include "Runtime/Component.h"
#include "Runtime/Entity.h"

namespace Nova
{
    InspectorWindow::InspectorWindow(): EditorWindow("Inspector Window")
    {
        m_Show = false;
    }

    void InspectorWindow::OnGui()
    {
        EditorWindow::OnGui();
        if (!m_Show) return;

        if (ImGui::Begin("Inspector Window", &m_Show))
        {
            Entity* entity = Selection::SelectedEntity;
            if (!entity)
            {
                ImGui::End();
                return;
            }

            const EntityHideFlags entityFlags = entity->GetHideFlags();
            if (entityFlags.Contains(EntityHideFlagBits::HideInInspector))
            {
                ImGui::End();
                return;
            }

            char nameBuffer[256] = { 0 };
            const StringView entityName = entity->GetObjectName();
            std::copy(entityName.begin(), entityName.end(), nameBuffer);

            ImGui::PushID(entity->GetUUID());
            ImGui::PushItemWidth(200);
            if (ImGui::InputText("##", nameBuffer, 256, ImGuiInputTextFlags_EnterReturnsTrue))
                entity->SetObjectName(String(nameBuffer));
            ImGui::PopItemWidth();

            ImGui::SameLine();
            ImGui::AddComponent(entity);
            ImGui::SameLine();
            bool entityEnabled = entity->IsEnabled();
            if (ImGui::Checkbox("Enabled", &entityEnabled))
                entity->SetEnabled(entityEnabled);
            ImGui::PopID();

            ImGui::TextUnformatted(*StringFormat("UUID: {}", entity->GetUUID().GetString()));

            static bool showContextMenu = false;
            entity->ForEach([](Component* component)
            {
                if (component->GetComponentFlags().Contains(ComponentFlagBits::HideInInspector))
                    return;

                ImGui::PushID(component->GetUuid());

                //if (ImGui::TreeNode(*Component->GetName()))
                //{
                //    Component->OnInspectorGUI(IO);
                //    ImGui::TreePop();
                //}
                ImGui::PopID();

                if (ImGui::IsItemClicked((ImGuiMouseButton)MouseButton::Right))
                {
                    showContextMenu = true;
                    ImGui::OpenPopup("ComponentContextMenu");
                    Selection::SelectedObject = component;
                }
            });


            if (showContextMenu && ImGui::BeginPopup("ComponentContextMenu"))
            {
                const char* options[]
                {
                    "Delete"
                };

                ImGui::SeparatorText("Context Menu");
                for (size_t i = 0; i < std::size(options); i++)
                {
                    if (ImGui::Selectable(options[i]))
                    {
                        Component* component = (Component*)Selection::SelectedObject;

                        switch (i)
                        {
                        case 0: entity->RemoveComponent(component); break;
                        default: break;
                        }
                        showContextMenu = false;
                    }
                }

                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }
}
