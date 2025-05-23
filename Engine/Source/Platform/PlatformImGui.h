#pragma once
#include "Runtime/Types.h"
#include <imgui_internal.h>


namespace Nova
{
    struct Vector3; class UUID;
}

namespace ImGui
{
    IMGUI_API void PushID(const Nova::UUID& Uuid);
    IMGUI_API ImGuiID GetID(const Nova::UUID& Uuid);
}

namespace ImGuizmo
{
    IMGUI_API void DecomposeMatrixToComponents(const Nova::f32* matrix, Nova::Vector3& translation, Nova::Vector3& rotation, Nova::Vector3& scale);
    IMGUI_API void RecomposeMatrixFromComponents(const Nova::Vector3& translation, const Nova::Vector3& rotation, const Nova::Vector3& scale, Nova::f32* matrix);
}