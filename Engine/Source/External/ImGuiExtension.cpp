#pragma once
#include "ImGuiExtension.h"
#include <cstdint>

#include "Runtime/Component.h"
#include "Runtime/Uuid.h"

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
}
