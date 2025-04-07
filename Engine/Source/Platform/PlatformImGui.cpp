#include "PlatformImGui.h"
#include "Core/Application.h"
#include "Core/Window.h"
#include "Math/LinearAlgebra.h"
#include <ImGuizmo/ImGuizmo.h>

namespace ImGui
{
    void PushID(const Nova::UUID& guid)
    {
        const u64* begin = guid.GetValues();
        const u64* end = begin + 2;
        PushID((const char*)begin, (const char*)end);
    }

    ImGuiID GetID(const Nova::UUID& guid)
    {
        ImGuiWindow* window = GImGui->CurrentWindow;
        const u64* begin = guid.GetValues();
        const u64* end = begin + 2;
        return window->GetID((const char*)begin, (const char*)end);
    }  
}

void ImGuizmo::DecomposeMatrixToComponents(const f32* matrix, Nova::Vector3& translation, Nova::Vector3& rotation, Nova::Vector3& scale)
{
    ImGuizmo::DecomposeMatrixToComponents(matrix, translation.ValuePtr(), rotation.ValuePtr(), scale.ValuePtr());
}

void ImGuizmo::RecomposeMatrixFromComponents(const Nova::Vector3& translation, const Nova::Vector3& rotation, const Nova::Vector3& scale, f32* matrix)
{
    ImGuizmo::RecomposeMatrixFromComponents(translation.ValuePtr(), rotation.ValuePtr(), scale.ValuePtr(), matrix);
}