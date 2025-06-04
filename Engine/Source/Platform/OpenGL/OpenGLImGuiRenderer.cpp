#include "OpenGLImGuiRenderer.h"
#include "Platform/PlatformRenderer.h"
#include "Runtime/Window.h"
#include "Runtime/DesktopWindow.h"
#include "Runtime/Application.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <ImGuizmo.h>


namespace Nova
{
    bool OpenGLImGuiRenderer::Initialize(Application* Application)
    {
        ImGuiRenderer::Initialize(Application);
        Window* Window = Application->GetWindow();

        if (DesktopWindow* Desktop = Window->As<DesktopWindow>())
        {
            if(!ImGui_ImplGlfw_InitForOpenGL(Desktop->GetHandle(), true))
            {
                NOVA_IMGUI_ERROR("Failed to initialize ImGui for GLFW (OpenGL)!");
                return false;
            }
        }


        if(!ImGui_ImplOpenGL3_Init("#version 460 core"))
        {
            NOVA_IMGUI_ERROR("Failed to initialize ImGui for OpenGL!");
            return false;
        }
        return true;
    }

    void OpenGLImGuiRenderer::Shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        Window* Window = m_Application->GetWindow();
        if (DesktopWindow* Desktop = Window->As<DesktopWindow>())
        {
            ImGui_ImplGlfw_Shutdown();
        }
        ImGui::DestroyContext(m_Context);
    }

    void OpenGLImGuiRenderer::BeginFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        Window* Window = m_Application->GetWindow();
        if (DesktopWindow* Desktop = Window->As<DesktopWindow>())
        {
            ImGui_ImplGlfw_NewFrame();
        }
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }
    

    void OpenGLImGuiRenderer::Render()
    {
        ImGui::Render();
        ImDrawData* Data = ImGui::GetDrawData();
        ImGui_ImplOpenGL3_RenderDrawData(Data);
    }
}
