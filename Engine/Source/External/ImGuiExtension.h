#pragma once
#include <imgui.h>

namespace Nova
{
    class Uuid;
    class Component;
}

namespace ImGui
{
    void IMGUI_API PushID(const Nova::Uuid& id);
    void IMGUI_API PushID(const Nova::Component* component);
}