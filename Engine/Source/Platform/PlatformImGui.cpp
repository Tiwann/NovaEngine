#include "PlatformImGui.h"
#include "Core/Application.h"
#include "Core/Window.h"
#include "Math/LinearAlgebra.h"
#include <ImGuizmo/ImGuizmo.h>

namespace ImGui
{
    void PushID(const Hydro::UUID& guid)
    {
        const u64* begin = guid.GetValues();
        const u64* end = begin + 2;
        PushID((const char*)begin, (const char*)end);
    }

    ImGuiID GetID(const Hydro::UUID& guid)
    {
        ImGuiWindow* window = GImGui->CurrentWindow;
        const u64* begin = guid.GetValues();
        const u64* end = begin + 2;
        return window->GetID((const char*)begin, (const char*)end);
    }  
}

void ImGuizmo::DecomposeMatrixToComponents(const f32* matrix, Hydro::Vector3& translation, Hydro::Vector3& rotation, Hydro::Vector3& scale)
{
    ImGuizmo::DecomposeMatrixToComponents(matrix, translation.ValuePtr(), rotation.ValuePtr(), scale.ValuePtr());
}

void ImGuizmo::RecomposeMatrixFromComponents(const Hydro::Vector3& translation, const Hydro::Vector3& rotation, const Hydro::Vector3& scale, f32* matrix)
{
    ImGuizmo::RecomposeMatrixFromComponents(translation.ValuePtr(), rotation.ValuePtr(), scale.ValuePtr(), matrix);
}