#pragma once
#include "EditorWindow.h"

namespace Nova
{
    class Scene;

    class HierarchyWindow : public EditorWindow
    {
    public:
        explicit HierarchyWindow();
        void OnGui() override;
    };
}