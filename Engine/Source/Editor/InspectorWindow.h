#pragma once
#include "EditorWindow.h"

namespace Nova
{
    class InspectorWindow : public EditorWindow
    {
    public:
        explicit InspectorWindow();

        void OnGui() override;
    };
}