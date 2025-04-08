#pragma once
#include "EditorWindow.h"

namespace Nova
{
    class PhysicsSettingsPanel : public EditorWindow
    {
    public:
        explicit PhysicsSettingsPanel() : EditorWindow("Physics Settings") { }
        void OnInspectorGUI(const ImGuiIO& IO) override;
    };
}
