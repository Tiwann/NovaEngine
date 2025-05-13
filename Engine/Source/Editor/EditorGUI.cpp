#include "EditorGUI.h"
#include "StyleVar.h"
#include "Runtime/Sprite.h"
#include "Containers/StringFormat.h"
#include "Runtime/Application.h"
#include "Components/Camera.h"
#include "Components/Audio/SoundEmitter.h"
#include "Components/Audio/SoundListener.h"
#include "Components/Physics/Box2D.h"
#include "Components/Physics/Box3D.h"
#include "Components/Physics/Circle2D.h"
#include "Components/Rendering/AmbientLight.h"
#include "Components/Rendering/DirectionalLight.h"
#include "Components/Rendering/StaticMeshRenderer.h"
#include "Components/Rendering/PointLight.h"
#include "Components/Rendering/SpriteRenderer.h"
#include "ResourceManager/TextureManager.h"

namespace Nova::UI
{
    void NewWindow(const StringView Label, bool& Opened, WindowFlags Flags, const Function<void()>& Content)
    {
        if(!Opened) return;
        if(ImGui::Begin(Label, &Opened, Flags.As<ImGuiWindowFlags>()))
        {
            if(Content) Content();
        }
        ImGui::End();
    }
    

    void Text(const StringView Str)
    {
        ImGui::Text(Str);
    }
    
    bool Button(const StringView Label, Vector2 Size, bool Enabled)
    {
        ImGui::BeginDisabled(!Enabled);
        const bool Result = ImGui::Button(Label, { Size.x, Size.y });
        ImGui::EndDisabled();
        return Result;
    }

    static void MenuItemRecursive(const TreeNode<MenuItem>& Node)
    {
        for (const auto& ChildNode : Node)
        {
            const MenuItem& Item = ChildNode.GetValue();
            if(!Item.Selected)
            {
                if (ChildNode.HasChildren())
                {
                    if (ImGui::BeginMenu(Item.Name.Data(), Item.Enabled))
                    {
                        if (Item.Callback)
                            Item.Callback();

                        for(const auto& SubChild : ChildNode)
                            MenuItemRecursive(SubChild);

                        ImGui::EndMenu();
                    }
                }
                else if (ImGui::MenuItem(Item.Name.Data(), nullptr, false, Item.Enabled))
                {
                    if (Item.Callback)
                        Item.Callback();

                    for(const auto& SubChild : ChildNode)
                        MenuItemRecursive(SubChild);
                }
            } else
            {
                if (ChildNode.HasChildren())
                {
                    if (ImGui::BeginMenu(Item.Name.Data(), Item.Enabled))
                    {
                        if (Item.Callback)
                            Item.Callback();

                        for(const auto& SubChild : ChildNode)
                            MenuItemRecursive(SubChild);

                        ImGui::EndMenu();
                    }
                }
                else if (ImGui::MenuItem(Item.Name.Data(), nullptr, Item.Selected, Item.Enabled))
                {
                    if (Item.Callback)
                        Item.Callback();

                    for(const auto& SubChild : ChildNode)
                        MenuItemRecursive(SubChild);
                }
            }
                        
        }
    }
    
    void MainMenuMenuBar(const TreeNode<MenuItem>& RootNode)
    {
        if (ImGui::BeginMainMenuBar())
        {
            for(const auto& FirstDegreeChild : RootNode)
            {
                const MenuItem& Item = FirstDegreeChild.GetValue();
                if (ImGui::BeginMenu(*Item.Name, Item.Enabled))
                {
                    MenuItemRecursive(FirstDegreeChild);
                    ImGui::EndMenu();
                }
            }
            ImGui::EndMainMenuBar();
        }
    }

    void AddComponent(Entity* Entity)
    {
        static bool ShowListBox = false;
        
        if(Button("Add Component"))
        {
            ShowListBox = true;
            ImGui::OpenPopup("AddComponent");
        }

        if(ShowListBox && ImGui::BeginPopup("AddComponent"))
        {
            const char* ComponentNames[]
            {
                "Camera",
                "Sound Emitter",
                "Sound Listener",
                "Sprite Renderer",
                "Box 2D",
                "Circle 2D",
                "Box 3D",
                "Model Renderer",
                "Directional Light",
                "Point Light",
                "Ambient Light"
            };


            ImGui::SeparatorText("Add Component");
            for(size_t i = 0; i < std::size(ComponentNames); i++)
            {
                if(ImGui::Selectable(ComponentNames[i]))
                {
                    switch (i)
                    {
                    case 0: Entity->AddComponent<Camera>(); break;
                    case 1: Entity->AddComponent<SoundEmitter>(); break;
                    case 2: Entity->AddComponent<SoundListener>(); break;
                    case 3: Entity->AddComponent<SpriteRenderer>(); break;
                    case 4: Entity->AddComponent<Box2D>(); break;
                    case 5: Entity->AddComponent<Circle2D>(); break;
                    case 6: Entity->AddComponent<Box3D>(); break;
                    case 7: Entity->AddComponent<StaticMeshRenderer>(); break;
                    case 8: Entity->AddComponent<DirectionalLight>(); break;
                    case 9: Entity->AddComponent<PointLight>(); break;
                    case 10: Entity->AddComponent<AmbientLight>(); break;
                    }
                    ShowListBox = false;
                }
            }
            
            ImGui::EndPopup();
        }
    }

    bool ItemClicked(MouseButton Button)
    {
        return ImGui::IsItemClicked((ImGuiMouseButton)Button);
    }

    bool Combo(StringView Name, int* Current, ArrayView<const char*> Names)
    {
        return ImGui::Combo(Name.Data(), Current, Names.Data(), (i32)Names.Count());
    }

    void Image(const Texture2D* InTexture, const Vector2& DrawSize)
    {
        ImGui::Image(InTexture->GetHandle(), ImVec2(DrawSize.x, DrawSize.y), ImVec2(0, 1), ImVec2(1, 0));
    }


    void Sprite(Nova::Sprite& InOutSprite)
    {
        if(ImGui::TreeNode("Sprite"))
        {
            if(InOutSprite.GetTexture())
            {
                UI::Image(InOutSprite.GetTexture(), InOutSprite.GetTexture()->GetSize());
            }

            if(Button("Load Sprite"))
            {
                const Path Filepath = File::OpenFileDialog("Choose an image file", "", DialogFilters::ImageFilters);
                TextureManager* Manager = g_Application->GetTextureManager();
                const std::string Name = Filepath.filename().string();
                Texture2D* NewTexture = Manager->Load(Name.c_str(), Filepath);
                InOutSprite = NewTexture->CreateSprite();
            }
            ImGui::TreePop();
        }
    }
}

