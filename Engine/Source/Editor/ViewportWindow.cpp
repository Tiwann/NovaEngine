#include "ViewportWindow.h"
#include "External/ImGuiExtension.h"
#include "Rendering/RenderDevice.h"
#include "Rendering/RenderTarget.h"
#include "Rendering/Sampler.h"
#include <imgui.h>
#include <imgui_impl_vulkan.h>

#include "Runtime/Application.h"


namespace Nova
{
    void ViewportWindow::OnInit()
    {
        EditorWindow::OnInit();

        SamplerCreateInfo samplerCreateInfo;
        samplerCreateInfo.device = m_Device;
        m_Sampler = m_Device->CreateSampler(samplerCreateInfo);
    }

    void ViewportWindow::OnDestroy()
    {
        EditorWindow::OnDestroy();
        m_Sampler->Destroy();
    }

    void ViewportWindow::SetRenderTarget(Ref<RenderTarget> renderTarget)
    {
        m_RenderTarget = renderTarget;
    }

    void ViewportWindow::OnGui()
    {
        EditorWindow::OnGui();
        if (ImGui::Begin(GetObjectName()))
        {
            Application& application = Application::GetCurrentApplication();
            Ref<ImGuiRenderer>& imguiRenderer = application.GetImGuiRenderer();
            Ref<RenderDevice> device = application.GetRenderDevice();
            imguiRenderer->DrawTexture(*m_RenderTarget->GetColorTexture(), m_RenderTarget->GetWidth(), m_RenderTarget->GetHeight());
            ImGui::End();
        }
    }
}
