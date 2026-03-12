#pragma once
#include "Runtime/Ref.h"
#include "Containers/StringView.h"
#include <imgui.h>

#include "Rendering/TextureView.h"


namespace Nova
{
    class UUID;
    class Component;
    class Entity;
    class ITexture;
    class Sampler;
}

using Nova::Ref;
using Nova::StringView;
using Nova::Entity;
using Nova::Ref;
using Nova::ITexture;
using Nova::Sampler;
using Nova::UUID;
using Nova::Component;

namespace ImGui
{
    bool IMGUI_API Begin(StringView name);
    void IMGUI_API PushID(const UUID& id);
    void IMGUI_API PushID(const Component* component);
    void IMGUI_API AddComponent(Entity* entity);
    void IMGUI_API ConvertColorToLinear(ImVec4& color);
    void IMGUI_API NovaStyle(ImGuiStyle* style = nullptr);
    void IMGUI_API Image(const Nova::TextureView& textureView, uint32_t width, uint32_t height);
}