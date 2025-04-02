#pragma once
#include "Runtime/Types.h"
#include <imgui_internal.h>


namespace Hydro
{
    struct Vector3; class UUID;
}

namespace ImGui
{
    IMGUI_API void PushID(const Hydro::UUID& guid);
    IMGUI_API ImGuiID GetID(const Hydro::UUID& guid);
}

namespace ImGuizmo
{
    IMGUI_API void DecomposeMatrixToComponents(const f32* matrix, Hydro::Vector3& translation, Hydro::Vector3& rotation, Hydro::Vector3& scale);
    IMGUI_API void RecomposeMatrixFromComponents(const Hydro::Vector3& translation, const Hydro::Vector3& rotation, const Hydro::Vector3& scale, f32* matrix);
}