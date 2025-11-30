#pragma once
#include "EditorWindow.h"

namespace Nova
{
    class InspectorWindow final : public EditorWindow
    {
    public:
        explicit InspectorWindow();

        void OnGui() override;
    };
}