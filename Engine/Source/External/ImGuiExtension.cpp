#pragma once
#include "ImGuiExtension.h"
#include "Components/Camera.h"
#include "Components/Audio/AudioListener.h"
#include "Components/Audio/AudioSource.h"
#include "Components/Physics/BoxComponent2D.h"
#include "Components/Physics/PlaneComponent2D.h"
#include "Components/Rendering/SpriteRenderer.h"
#include "Runtime/Component.h"
#include "Runtime/Entity.h"
#include "Runtime/Uuid.h"
#include <imgui_impl_vulkan.h>
#include <cstdint>

#include "Rendering/Device.h"
#include "Rendering/Vulkan/Sampler.h"
#include "Rendering/Vulkan/Texture.h"

namespace ImGui
{
    void PushID(const Nova::Uuid& id)
    {
        const uint64_t* begin = id.GetValues();
        const uint64_t* end = id.GetValues() + 2;
        PushID((const char*)begin, (const char*)end);
    }

    void PushID(const Nova::Component* component)
    {
        PushID(component->GetUuid());
    }

    void AddComponent(Nova::Entity* entity)
    {
        static bool showPopup = false;

        if(Button("Add Component"))
        {
            showPopup = true;
            OpenPopup("AddComponent");
        }

        if(showPopup && BeginPopup("AddComponent"))
        {
            const Nova::StringView componentNames[]
            {
                "Camera",
                "Audio Listener",
                "Audio Source",
                "Box Component 2D",
                "Plane Component 2D",
                "Sprite Renderer"
            };


            SeparatorText("Add Component");
            for(size_t i = 0; i < std::size(componentNames); i++)
            {
                if(Selectable(componentNames[i]))
                {
                    switch (i)
                    {
                    case 0: entity->AddComponent<Nova::Camera>(); break;
                    case 1: entity->AddComponent<Nova::AudioListener>(); break;
                    case 2: entity->AddComponent<Nova::AudioSource>(); break;
                    case 3: entity->AddComponent<Nova::BoxComponent2D>(); break;
                    case 4: entity->AddComponent<Nova::PlaneComponent2D>(); break;
                    case 5: entity->AddComponent<Nova::SpriteRenderer>(); break;
                    default: break;
                    }
                    showPopup = false;
                }
            }

            EndPopup();
        }
    }

    void NovaStyle(ImGuiStyle* style)
    {
        ImGuiStyle& __style = style ? *style : GetStyle();
        ImVec4* colors = __style.Colors;
        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
        colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
        colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

        __style.WindowPadding = ImVec2(8.00f, 8.00f);
        __style.FramePadding = ImVec2(5.00f, 2.00f);
        __style.CellPadding = ImVec2(6.00f, 6.00f);
        __style.ItemSpacing = ImVec2(6.00f, 6.00f);
        __style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
        __style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
        __style.IndentSpacing = 25;
        __style.ScrollbarSize = 15;
        __style.GrabMinSize = 10;
        __style.WindowBorderSize = 1;
        __style.ChildBorderSize = 1;
        __style.PopupBorderSize = 1;
        __style.FrameBorderSize = 1;
        __style.TabBorderSize = 1;
        __style.WindowRounding = 7;
        __style.ChildRounding = 4;
        __style.FrameRounding = 3;
        __style.PopupRounding = 4;
        __style.ScrollbarRounding = 9;
        __style.GrabRounding = 3;
        __style.LogSliderDeadzone = 4;
        __style.TabRounding = 4;
    }

    void Texture(Nova::Ref<Nova::Rendering::Texture> texture, Nova::Ref<Nova::Rendering::Sampler> sampler)
    {
        if (auto tex = texture.As<Nova::Vulkan::Texture>();
            auto texSampler = sampler.As<Nova::Vulkan::Sampler>())
        {
            const ImTextureID id = (ImTextureID)ImGui_ImplVulkan_AddTexture(texSampler->GetHandle(), tex->GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            Image(id, ImVec2(tex->GetWidth(), tex->GetHeight()));

        }
    }
}
