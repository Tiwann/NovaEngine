#pragma once
#include "Runtime/Ref.h"
#include "Containers/StringView.h"
#include <imgui.h>


namespace Nova
{
    class Uuid;
    class Component;
    class Entity;
    class Texture;
    class Sampler;
}

using Nova::Ref;
using Nova::StringView;
using Nova::Entity;
using Nova::Ref;
using Nova::Texture;
using Nova::Sampler;
using Nova::Uuid;
using Nova::Component;

namespace ImGui
{
    bool IMGUI_API Begin(StringView name);
    void IMGUI_API PushID(const Uuid& id);
    void IMGUI_API PushID(const Component* component);
    void IMGUI_API AddComponent(Entity* entity);
    void IMGUI_API ConvertColorToLinear(ImVec4& color);
    void IMGUI_API NovaStyle(ImGuiStyle* style = nullptr);
    void IMGUI_API Image(const Texture& texture, const Sampler& sampler);
}