#pragma once
#include "Runtime/Ref.h"
#include "Containers/StringView.h"
#include <imgui.h>


namespace Nova
{
    class Uuid;
    class Component;
    class Entity;
    namespace Rendering { class Texture; class Sampler; }
}

namespace ImGui
{
    bool IMGUI_API Begin(Nova::StringView name);
    void IMGUI_API PushID(const Nova::Uuid& id);
    void IMGUI_API PushID(const Nova::Component* component);
    void IMGUI_API AddComponent(Nova::Entity* entity);
    void IMGUI_API NovaStyle(ImGuiStyle* style = nullptr);
    void IMGUI_API Texture(Nova::Ref<Nova::Rendering::Texture> texture, Nova::Ref<Nova::Rendering::Sampler> sampler);
}