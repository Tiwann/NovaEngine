#include "EditorApplication.h"
#include "Runtime/EntryPoint.h"
#include <imgui.h>

#include "Rendering/CommandBuffer.h"

NOVA_DEFINE_APPLICATION_CLASS(Nova::EditorApplication);

namespace Nova
{
    ApplicationConfiguration EditorApplication::GetConfiguration() const
    {
        ApplicationConfiguration config;
        config.applicationName = "Nova Editor";
        config.windowFlags = WindowCreateFlagBits::Decorated | WindowCreateFlagBits::Show | WindowCreateFlagBits::Resizable | WindowCreateFlagBits::CreateAtCenter;
        config.windowWidth = 1600;
        config.windowHeight = 900;
        config.vsync = true;
        return config;
    }

    void EditorApplication::OnInit()
    {
        Application::OnInit();
    }

    void EditorApplication::OnUpdate(const float deltaTime)
    {
        Application::OnUpdate(deltaTime);
    }

    void EditorApplication::OnGUI()
    {
        Application::OnGUI();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open Scene"))
                {

                }

                if (ImGui::MenuItem("Close Scene"))
                {

                }

                if (ImGui::MenuItem("Save Scene"))
                {

                }

                if (ImGui::MenuItem("Exit"))
                {
                    Exit();
                }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void EditorApplication::OnRender(Rendering::CommandBuffer& cmdBuffer)
    {
        cmdBuffer.ClearColor(Color::Red, 0);
    }
}
