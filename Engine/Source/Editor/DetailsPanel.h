#pragma once
#include "EditorWindow.h"
#include "Runtime/LogCategory.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(DetailsPanel, "DETAILS PANEL")

namespace Nova
{
    class DetailsPanel : public EditorWindow
    {
    public:
        DetailsPanel(class Scene* InScene);
        void OnInspectorGUI(const ImGuiIO& IO) override;

    private:
        Scene* m_CurrentScene;
    };
}
