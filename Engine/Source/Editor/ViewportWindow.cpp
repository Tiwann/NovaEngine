#include "ViewportWindow.h"
#include "External/ImGuiExtension.h"
#include "Rendering/Vulkan/RenderTarget.h"
#include "Rendering/Vulkan/Sampler.h"
#include <imgui.h>
#include <imgui_impl_vulkan.h>

#include "Rendering/RenderDevice.h"
#include "Rendering/Vulkan/RenderDevice.h"


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

    void ViewportWindow::UpdateResources()
    {
        if (Ref<Vulkan::RenderTarget> rt = m_RenderTarget.As<Vulkan::RenderTarget>())
        {
            for (uint32_t i = 0; i < rt->GetImageCount(); i++)
            {
                if (m_TextureIds[0]) ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)m_TextureIds[0]);

                const Ref<Vulkan::Sampler> sampler = m_Sampler.As<Vulkan::Sampler>();
                const VkImageView imageView = rt->GetColorImageView(i);
                m_TextureIds[i] = (uint64_t)ImGui_ImplVulkan_AddTexture(sampler->GetHandle(), imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            }
        }
    }

    void ViewportWindow::OnGui()
    {
        EditorWindow::OnGui();
        if (ImGui::Begin(GetObjectName()))
        {
            if (Ref<Vulkan::RenderDevice> device = m_Device.As<Vulkan::RenderDevice>())
            {
                const uint32_t frameIndex = device->GetCurrentFrameIndex();
                ImGui::Image(ImTextureRef(m_TextureIds[frameIndex]), ImVec2(m_RenderTarget->GetWidth(), m_RenderTarget->GetHeight()));
            }
            ImGui::End();
        }
    }
}
