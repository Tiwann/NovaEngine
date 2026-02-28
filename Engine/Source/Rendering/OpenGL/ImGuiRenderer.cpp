#include "ImGuiRenderer.h"
#include "Runtime/DesktopWindow.h"
#include "RenderDevice.h"
#include "Swapchain.h"
#include <imgui.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>

namespace Nova::OpenGL
{
    bool ImGuiRenderer::Initialize(const ImGuiRendererCreateInfo& createInfo)
    {
        if (!Nova::ImGuiRenderer::Initialize(createInfo))
            return false;

        if (DesktopWindow* desktopWindow = dynamic_cast<DesktopWindow*>(createInfo.window))
        {
            if(!ImGui_ImplGlfw_InitForOpenGL(desktopWindow->GetHandle(), true))
                return false;
        }

        if (!ImGui_ImplOpenGL3_Init("#version 460 core"))
            return false;

        m_Device = static_cast<RenderDevice*>(createInfo.device);
        return true;
    }

    void ImGuiRenderer::Destroy()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(m_Context);
    }

    void ImGuiRenderer::BeginFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiRenderer::EndFrame()
    {
        ImGui::EndFrame();
    }

    void ImGuiRenderer::Render(Nova::CommandBuffer& commandBuffer)
    {
        ImGui::Render();
        ImDrawData* drawData = ImGui::GetDrawData();
        if (!drawData) return;

        const Swapchain* swapchain = static_cast<Swapchain*>(m_Device->GetSwapchain());
        const uint32_t width = swapchain->GetWidth();
        const uint32_t height = swapchain->GetHeight();
        // WAITING FOR VIEWPORT CONCEPT AND CMDBUFFER OPENGL IMPLEMENTATION
        glViewport(0, 0, width, height);
        glScissor(0, 0, width, height);
        ImGui_ImplOpenGL3_RenderDrawData(drawData);
    }

    void ImGuiRenderer::DrawTexture(const Nova::Texture& texture, uint32_t width, uint32_t height)
    {

    }
}
