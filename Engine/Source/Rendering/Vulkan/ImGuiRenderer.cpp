#include "ImGuiRenderer.h"
#include "Runtime/Memory.h"
#include "Device.h"
#include "Runtime/DesktopWindow.h"

#include <print>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace Nova::Vulkan
{
    bool ImGuiRenderer::Initialize(const Rendering::ImGuiRendererCreateInfo& createInfo)
    {
        if (!Rendering::ImGuiRenderer::Initialize(createInfo))
            return false;

        if (DesktopWindow* desktopWindow = dynamic_cast<DesktopWindow*>(createInfo.window))
        {
            if(!ImGui_ImplGlfw_InitForVulkan(desktopWindow->GetHandle(), true))
                return false;
        }

        Device* device = (Device*)createInfo.device;
        const Swapchain* swapchain = device->GetSwapchain();
        const Queue* graphicsQueue = device->GetGraphicsQueue();

        ImGui_ImplVulkan_InitInfo initInfo;
        Memory::Memzero(initInfo);

        initInfo.Instance = device->GetInstance();
        initInfo.Device = device->GetHandle();
        initInfo.PhysicalDevice = device->GetPhysicalDevice();
        initInfo.Queue = graphicsQueue->GetHandle();
        initInfo.Allocator = nullptr;
        initInfo.ImageCount = swapchain->GetImageCount();
        initInfo.QueueFamily = graphicsQueue->GetIndex();
        initInfo.MinImageCount = swapchain->GetImageCount();
        initInfo.DescriptorPool = nullptr;
        initInfo.DescriptorPoolSize = 32;
        initInfo.UseDynamicRendering = true;

        initInfo.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        initInfo.PipelineRenderingCreateInfo.viewMask = 0;
        initInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
        initInfo.PipelineRenderingCreateInfo.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
        initInfo.PipelineRenderingCreateInfo.stencilAttachmentFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
        constexpr VkFormat ColorAttachment = VK_FORMAT_R8G8B8A8_UNORM;
        initInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats = &ColorAttachment;
        initInfo.MSAASamples = (VkSampleCountFlagBits)createInfo.sampleCount;

        initInfo.CheckVkResultFn = [](const VkResult result)
        {
            if(result != VK_SUCCESS)
            {
                std::println(std::cerr, "Something went wrong.");
            }
        };

        if(!ImGui_ImplVulkan_Init(&initInfo))
            return false;

        m_Device = device;
        return true;
    }

    void ImGuiRenderer::Destroy()
    {
        m_Device->WaitIdle();
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(m_Context);
    }

    void ImGuiRenderer::BeginFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiRenderer::EndFrame()
    {
        ImGui::EndFrame();
    }

    void ImGuiRenderer::Render(Rendering::CommandBuffer& commandBuffer)
    {
        ImGui::Render();
        ImDrawData* drawData = ImGui::GetDrawData();
        if (!drawData) return;
        const VkCommandBuffer cmdBuffer = ((CommandBuffer&)commandBuffer).GetHandle();
        const uint32_t width = m_Device->GetSwapchain()->GetWidth();
        const uint32_t height = m_Device->GetSwapchain()->GetHeight();
        commandBuffer.SetViewport(0, 0, width, height, 0.0f, 1.0f);
        commandBuffer.SetScissor(0, 0, width, height);
        ImGui_ImplVulkan_RenderDrawData(drawData, cmdBuffer);
    }
}
