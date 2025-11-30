#include "HierarchyWindow.h"

#include "Selection.h"
#include "Runtime/Entity.h"
#include "Runtime/Scene.h"
#include "External/ImGuiExtension.h"
#include "Input/MouseButton.h"

namespace Nova
{
    static void DrawEntity(const Entity* entity)
    {
        ImGui::PushID(entity->GetUUID());
        if (ImGui::TreeNode(*entity->GetObjectName()))
        {
            for (size_t childIndex = 0; childIndex < entity->GetChildCount(); ++childIndex)
            {
                const Entity* child = entity->GetChild(childIndex);
                if (!child) continue;
                DrawEntity(child);
            }
            ImGui::TreePop();
        }
        ImGui::PopID();

        if (ImGui::IsItemClicked((ImGuiMouseButton)MouseButton::Left))
            Selection::SelectedEntity = (Entity*)entity;
    }

    HierarchyWindow::HierarchyWindow(): EditorWindow("Hierarchy Window")
    {
        m_Show = true;
    }

    void HierarchyWindow::OnGui()
    {
        Scene* scene = Selection::SelectedScene;
        if (!m_Show) return;

        if (ImGui::Begin("Hierarchy", &m_Show))
        {
            if (!scene)
            {
                ImGui::End();
                return;
            }

            if(ImGui::Button("Create Entity"))
                scene->CreateEntity("Empty Entity");

            for (const Entity* entity : scene->GetEntities())
            {
                if (!entity->HasParent())
                    DrawEntity(entity);
            }
        }
        ImGui::End();
    }
}
