#pragma once
#include "Runtime/Application.h"
#include "Runtime/Log.h"
#include "Runtime/LogCategory.h"
#include "Runtime/Project.h"
#include "Runtime/SharedPointer.h"
#include "Editor/EditorWindow.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(NovaEditor, "NOVA EDITOR")

namespace Nova
{
    class HelloTriangle final : public Application
    {
    public:
        explicit HelloTriangle(const Array<const char*>& Arguments);
        ApplicationConfiguration CreateConfiguration() const override;
        void OnInit() override;
        void OnExit() override;
        void OnRender(Renderer* Renderer) override;
        void OnUpdate(f32 Delta) override;

        void OpenProject(const Path& ProjectPath);
    private:
        SharedPtr<Project> m_CurrentProject;
        Array<EditorWindow*> m_EditorWindows;
        ImGuiRenderer* m_ImGuiRenderer = nullptr;
    };
}
