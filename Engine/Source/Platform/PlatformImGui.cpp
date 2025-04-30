#include "PlatformImGui.h"
#include "Math/LinearAlgebra.h"
#include "Runtime/Types.h"
#include "Runtime/UUID.h"
#include <ImGuizmo.h>

namespace ImGui
{
    void PushID(const Nova::UUID& Uuid)
    {
        const Nova::u64* begin = Uuid.GetValues();
        const Nova::u64* end = begin + 2;
        PushID((const char*)begin, (const char*)end);
    }

    ImGuiID GetID(const Nova::UUID& Uuid)
    {
        ImGuiWindow* window = GImGui->CurrentWindow;
        const Nova::u64* begin = Uuid.GetValues();
        const Nova::u64* end = begin + 2;
        return window->GetID((const char*)begin, (const char*)end);
    }  
}

void ImGuizmo::DecomposeMatrixToComponents(const Nova::f32* matrix, Nova::Vector3& translation, Nova::Vector3& rotation, Nova::Vector3& scale)
{
    ImGuizmo::DecomposeMatrixToComponents(matrix, translation.ValuePtr(), rotation.ValuePtr(), scale.ValuePtr());
}

void ImGuizmo::RecomposeMatrixFromComponents(const Nova::Vector3& translation, const Nova::Vector3& rotation, const Nova::Vector3& scale, Nova::f32* matrix)
{
    ImGuizmo::RecomposeMatrixFromComponents(translation.ValuePtr(), rotation.ValuePtr(), scale.ValuePtr(), matrix);
}