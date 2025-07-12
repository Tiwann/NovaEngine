#include "SceneHierarchyPanel.h"
#include "EditorGUI.h"
#include "Selection.h"
#include "ViewportPanel.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/Physics/RigidBody2D.h"
#include "Runtime/Application.h"
#include "Rendering/Renderer.h"
#include "Runtime/Scene.h"
#include <ImGuizmo.h>


namespace Nova
{
    SceneHierarchyPanel::SceneHierarchyPanel(Scene* InScene) : EditorWindow("Scene Hierarchy"), m_CurrentScene(InScene)
    {
        m_Opened = true;
    }

    static void DrawEntity(Entity* Entity)
    {
        if (Entity->GetHideFlags().Contains(EntityHideFlagBits::HideInHierarachy))
            return;
        
        const char* Name = Entity->GetName().IsEmpty() ? "##" : *Entity->GetName();

        ImGui::PushID(Entity->GetUUID());
        if(ImGui::TreeNode(Name))
        {
            if(Entity->HasChildren())
            {
                auto* Child = Entity->GetChild(0);
                DrawEntity(Child);
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
    
    void SceneHierarchyPanel::OnInspectorGUI(const ImGuiIO& IO)
    {
        EditorWindow::OnInspectorGUI(IO);
        if(!m_Opened) return;

        if(ImGui::Begin(*m_Name, &m_Opened))
        {
            if(ImGui::Button("Create Entity"))
            {
                m_CurrentScene->CreateEntity("Empty Entity");    
            }

            static bool ShowContextMenu = false;
            
            m_CurrentScene->ForEach([this](const EntityHandle Handle)
            {
                Entity* Entity = Handle.GetEntity();
                DrawEntity(Entity);
                if(UI::ItemClicked(MouseButton::Left))
                {
                    NOVA_LOG(SceneHierarchyPanel, Verbosity::Warning, "Item Clicked: {}", Entity->GetName());
                    Selection::SetEntity(Handle);
                }

                
                if(UI::ItemClicked(MouseButton::Right))
                {
                    ShowContextMenu = true;
                    ImGui::OpenPopup("ContextMenu");
                    Selection::SetEntity(Handle);
                }
            });

            if (ShowContextMenu && ImGui::BeginPopup("ContextMenu"))
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
                        EntityHandle SelectedEntity = Selection::GetEntity();
                        if(!SelectedEntity.IsValid()) break;
                            
                        switch (i)
                        {
                        case 0:
                            m_CurrentScene->DestroyEntity(SelectedEntity);
                            Selection::SetEntity(EntityHandle::NullHandle);
                        }
                        ShowContextMenu = false;
                    }
                }

                ImGui::EndPopup();
            }

#ifdef NOVA_USE_IMGUIZMO
            if(Entity* Entity = Selection::GetEntity())
            {
                const Application* App = g_Application;
                const Vector2 ViewportSize = App->GetViewportPanel()->GetSize();
                const Vector2 ViewportPos = App->GetViewportPanel()->GetPosition();
                const Camera* Cam = App->GetRendererBackend()->GetCurrentCamera();
                if(!Cam) return;

                ImGuizmo::SetOrthographic(Cam->Settings.Projection == CameraProjectionType::Orthographic);
                ImGuizmo::SetDrawlist(App->GetViewportPanel()->GetDrawList());
                ImGuizmo::SetRect(ViewportPos.x, ViewportPos.y, ViewportSize.x, ViewportSize.y);

                Transform* Transform = Entity->GetTransform();
                Matrix4 ModelMatrix = Transform->GetWorldSpaceMatrix();
                Matrix4 ViewMatrix = Cam->GetViewMatrix();
                Matrix4 ProjectionMatrix = Cam->GetProjectionMatrix();
                
                ImGuizmo::Manipulate(*ViewMatrix, *ProjectionMatrix, ImGuizmo::UNIVERSAL, ImGuizmo::WORLD, *ModelMatrix);
                if(ImGuizmo::IsUsing())
                {
                    Vector3 Pos, Rot, Sc;
                    ImGuizmo::DecomposeMatrixToComponents(*ModelMatrix, Pos, Rot, Sc);
                    Transform->SetPosition(Pos);
                    Transform->SetRotation(Rot);
                    Transform->SetScale(Sc);

                    if(RigidBody2D* Shape = Entity->GetComponent<RigidBody2D>())
                    {
                        Shape->SetPosition(Pos);
                        Shape->SetRotation(Rot.z);
                        Shape->RecreatePhysicsState();
                    }
                }
            }
#endif
        }
        ImGui::End();
        

       
    }
}
