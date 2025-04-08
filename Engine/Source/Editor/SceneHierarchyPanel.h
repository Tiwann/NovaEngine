#pragma once
#include "EditorWindow.h"
#include "Runtime/LogCategory.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(SceneHierarchyPanel, "SCENE HIERARCHY PANEL")

namespace Nova
{
    class Scene;
    
    class SceneHierarchyPanel : public EditorWindow
    {
    public:
        SceneHierarchyPanel(Scene* InScene);
        void OnInspectorGUI(const ImGuiIO& IO) override;
    
    private:
        Scene* m_CurrentScene;
    };
}


