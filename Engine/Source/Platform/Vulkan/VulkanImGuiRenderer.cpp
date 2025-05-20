#include "VulkanImGuiRenderer.h"
#include "Platform/PlatformRenderer.h"
#include "Runtime/Window.h"
#include "Runtime/Application.h"
#include "Platform/PlatformRenderer.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <GLFW/glfw3.h>
#include <ImGuizmo.h>

#include "VulkanCommandBuffer.h"
#include "VulkanSwapchain.h"


namespace Nova
{
    bool VulkanImGuiRenderer::Initialize(Application* Application)
    {
        ImGuiRenderer::Initialize(Application);
        Window* Window = Application->GetWindow();
        GLFWwindow* NativeWindow = Window->GetNativeWindow();
        
        if(!ImGui_ImplGlfw_InitForVulkan(NativeWindow, true))
        {
            NOVA_IMGUI_ERROR("Failed to initialize ImGui for GLFW (Vulkan)!");
            return false;
        }

        VulkanRenderer* Renderer = Application->GetRenderer<VulkanRenderer>();
        ImGui_ImplVulkan_InitInfo InitInfo;
        Memory::Memzero(InitInfo);
        
        InitInfo.Instance = Renderer->GetInstance();
        InitInfo.Device = Renderer->GetDevice();
        InitInfo.PhysicalDevice = Renderer->GetPhysicalDevice();
        InitInfo.Queue = Renderer->GetGraphicsQueue();
        InitInfo.Allocator = nullptr;
        InitInfo.ImageCount = (u32)Renderer->GetSwapchain()->GetImageCount();
        InitInfo.QueueFamily = Renderer->GetGraphicsQueueFamily();
        InitInfo.MinImageCount = (u32)Renderer->GetSwapchain()->GetImageCount();
        InitInfo.DescriptorPool = Renderer->GetDescriptorPool();
        InitInfo.UseDynamicRendering = true;
        InitInfo.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        InitInfo.PipelineRenderingCreateInfo.viewMask = 0;
        InitInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
        InitInfo.PipelineRenderingCreateInfo.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
        InitInfo.PipelineRenderingCreateInfo.stencilAttachmentFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
        constexpr VkFormat ColorAttachment = VK_FORMAT_R8G8B8A8_UNORM;
        InitInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats = &ColorAttachment;
        
        InitInfo.CheckVkResultFn = [](VkResult Result)
        {
            if(Result != VK_SUCCESS)
            {
                NOVA_IMGUI_ERROR("Something went wrong.");
                g_Application->RequireExit(ExitCode::Error);
            }
        };
        
        if(!ImGui_ImplVulkan_Init(&InitInfo))
        {
            NOVA_LOG(Application, Verbosity::Info, "Failed to initialize ImGui for Vulkan!");
            return false;
        }
        return true;
    }

    void VulkanImGuiRenderer::Shutdown()
    {
        const VulkanRenderer* Renderer = m_Application->GetRenderer<VulkanRenderer>();
        Renderer->WaitIdle();
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(m_Context);
    }

    void VulkanImGuiRenderer::BeginFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }
    

    void VulkanImGuiRenderer::Render()
    {
        ImGui::Render();
        ImDrawData* Data = ImGui::GetDrawData();
        const VulkanRenderer* Renderer = m_Application->GetRenderer<VulkanRenderer>();
        const VkCommandBuffer CommandBuffer = Renderer->GetCurrentCommandBuffer()->GetHandle();
        ImGui_ImplVulkan_RenderDrawData(Data, CommandBuffer);
    }
}
