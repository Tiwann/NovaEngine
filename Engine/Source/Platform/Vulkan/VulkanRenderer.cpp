#include "VulkanRenderer.h"
#include "Runtime/Application.h"
#include "Runtime/Window.h"
#include "Runtime/Log.h"
#include "Runtime/LogVerbosity.h"
#include "Runtime/PopupMessage.h"
#include "Containers/ScopedPointer.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/Shader.h"
#include <GLFW/glfw3.h>

#include "VulkanCommandBuffer.h"
#include "VulkanFence.h"
#include "VulkanIndexBuffer.h"
#include "VulkanPipeline.h"
#include "VulkanRenderTarget.h"
#include "VulkanSemaphore.h"
#include "VulkanShader.h"
#include "VulkanSwapchain.h"
#include "VulkanUniformBuffer.h"
#include "VulkanVertexBuffer.h"
#include "Platform/Vulkan/VulkanCommandPool.h"
#include "Rendering/CommandBuffer.h"
#include "Rendering/Multisample.h"
#include "Rendering/RenderTarget.h"
#include "Rendering/Swapchain.h"

namespace Nova
{
    #if defined(NOVA_DEBUG) || defined(NOVA_DEV)
    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugMessageCallback(
        const VkDebugUtilsMessageSeverityFlagBitsEXT Severity,
        const VkDebugUtilsMessageTypeFlagsEXT,
        const VkDebugUtilsMessengerCallbackDataEXT* Data,
        void*)
    {
        if (Severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            NOVA_VULKAN_ERROR("Vulkan Error: {}", Data->pMessage);
            return false;
        }

        if (Severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            NOVA_VULKAN_WARNING("Vulkan Warning: {}", Data->pMessage);
            return false;
        }
        return true;
    }
    #endif


    VulkanRenderer::VulkanRenderer(Application* Owner) : Renderer(Owner, GraphicsApi::Vulkan)
    {

    }

    bool VulkanRenderer::Initialize()
    {
        const i32 WindowWidth = g_Application->GetWindow()->GetWidth<i32>();
        const i32 WindowHeight = g_Application->GetWindow()->GetHeight<i32>();

        //////////////////////////////////////////////////////////////////////////////////////////
        /// INSTANCE CREATE
        //////////////////////////////////////////////////////////////////////////////////////////
        {
            VkApplicationInfo ApplicationInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
            ApplicationInfo.apiVersion = VK_API_VERSION_1_4;
            ApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            ApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            ApplicationInfo.pApplicationName = *m_Application->GetConfiguration().AppName;
            ApplicationInfo.pEngineName = "Nova Engine";

            u32 GlfwExtensionsCount;
            const char** GlfwExtensions = glfwGetRequiredInstanceExtensions(&GlfwExtensionsCount);

            Array<const char*> Extensions;
            Extensions.Add(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);
            Extensions.Add(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
            Extensions.AddRange(GlfwExtensions, GlfwExtensionsCount);

            #if defined(NOVA_DEBUG) || defined(NOVA_DEV)
            Extensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            #endif

            Array<const char*> Layers;
            Layers.Add(VK_LAYER_KHRONOS_VALIDATION_NAME);


            VkInstanceCreateInfo InstanceCreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
            InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;
            InstanceCreateInfo.enabledExtensionCount = (u32)Extensions.Count();
            InstanceCreateInfo.ppEnabledExtensionNames = Extensions.Data();
            InstanceCreateInfo.enabledLayerCount = (u32)Layers.Count();
            InstanceCreateInfo.ppEnabledLayerNames = Layers.Data();

            if (VK_FAILED(vkCreateInstance(&InstanceCreateInfo, nullptr, &m_Instance)))
            {
                NOVA_VULKAN_ERROR("Failed to create instance!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }

#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
            m_FunctionPointers.vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
            m_FunctionPointers.vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
#endif
            m_FunctionPointers.vkCmdSetColorBlendEnableEXT = (PFN_vkCmdSetColorBlendEnableEXT)vkGetInstanceProcAddr(m_Instance, "vkCmdSetColorBlendEnableEXT");
            m_FunctionPointers.vkCmdSetColorBlendEquationEXT = (PFN_vkCmdSetColorBlendEquationEXT)vkGetInstanceProcAddr(m_Instance, "vkCmdSetColorBlendEquationEXT");
            m_FunctionPointers.vkCreateShadersEXT = (PFN_vkCreateShadersEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateShadersEXT");
            m_FunctionPointers.vkDestroyShaderEXT = (PFN_vkDestroyShaderEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyShaderEXT");
            m_FunctionPointers.vkCmdBindShadersEXT = (PFN_vkCmdBindShadersEXT)vkGetInstanceProcAddr(m_Instance, "vkCmdBindShadersEXT");
            m_FunctionPointers.vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(m_Instance, "vkSetDebugUtilsObjectNameEXT");
        }

        //////////////////////////////////////////////////////////////////////////////////////////
        /// DEBUG MESSENGER CREATE
        //////////////////////////////////////////////////////////////////////////////////////////
        #if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        {
            constexpr VkDebugUtilsMessageSeverityFlagsEXT SeverityFlags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
            constexpr VkDebugUtilsMessageTypeFlagsEXT TypeFlags = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;

            VkDebugUtilsMessengerCreateInfoEXT DebugMessengerCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
            DebugMessengerCreateInfo.pfnUserCallback = VulkanDebugMessageCallback;
            DebugMessengerCreateInfo.messageSeverity = SeverityFlags;
            DebugMessengerCreateInfo.messageType = TypeFlags;

            if (VK_FAILED(m_FunctionPointers.vkCreateDebugUtilsMessengerEXT(m_Instance, &DebugMessengerCreateInfo, nullptr, &m_DebugMessenger)))
            {
                NOVA_VULKAN_ERROR("Failed to create debug messenger!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }
        }
        #endif

        //////////////////////////////////////////////////////////////////////////////////////////
        /// DEVICE CREATE
        //////////////////////////////////////////////////////////////////////////////////////////
        {
            u32 AvailableDevicesCount;
            if (VK_FAILED(vkEnumeratePhysicalDevices(m_Instance, &AvailableDevicesCount, nullptr)))
            {
                NOVA_VULKAN_ERROR("Failed to enumerate physical devices!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }

            ScopedBuffer<VkPhysicalDevice> AvailablePhysicalDevices(AvailableDevicesCount);
            if (VK_FAILED(vkEnumeratePhysicalDevices(m_Instance, &AvailableDevicesCount, AvailablePhysicalDevices.GetData())))
            {
                NOVA_VULKAN_ERROR("Failed to enumerate physical devices!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }

            for (VkPhysicalDevice PhysicalDevice : AvailablePhysicalDevices)
            {
                VkPhysicalDeviceProperties Properties;
                vkGetPhysicalDeviceProperties(PhysicalDevice, &Properties);
                VkPhysicalDeviceFeatures Features;
                vkGetPhysicalDeviceFeatures(PhysicalDevice, &Features);
                if (Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && Features.samplerAnisotropy == (VkBool32)true)
                {
                    m_PhysicalDevice = PhysicalDevice;
                    break;
                }
            }

            if (!m_PhysicalDevice)
            {
                NOVA_VULKAN_ERROR("Failed to pick physical device!");
                m_Application->RequireExit(ExitCode::Error);

                ScopedPointer<PopupMessage> Message = PopupMessage::Create("Fatal Error", "No suitable GPU found!", PopupMessageResponse::OK, PopupMessageIcon::Error);
                Message->Show();
                return false;
            }

            if (VK_FAILED(glfwCreateWindowSurface(m_Instance, m_Application->GetWindow()->GetNativeWindow(), nullptr, &m_Surface)))
            {
                NOVA_VULKAN_ERROR("Failed to create window surface!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }

            u32 QueueFamilyPropertiesCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties2(m_PhysicalDevice, &QueueFamilyPropertiesCount, nullptr);
            ScopedBuffer<VkQueueFamilyProperties2> QueueFamilyProperties(QueueFamilyPropertiesCount);
            for (u32 i = 0; i < QueueFamilyProperties.Count(); ++i)
            {
                QueueFamilyProperties[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
            }
            vkGetPhysicalDeviceQueueFamilyProperties2(m_PhysicalDevice, &QueueFamilyPropertiesCount, QueueFamilyProperties.GetData());

            for (u32 i = 0; i < QueueFamilyProperties.Count(); ++i)
            {
                if (QueueFamilyProperties[i].queueFamilyProperties.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT))
                {
                    m_GraphicsQueueIndex = i;
                    break;
                }
            }

            for (u32 i = 0; i < QueueFamilyProperties.Count(); ++i)
            {
                VkBool32 SupportsSurface = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, m_Surface, &SupportsSurface);
                if (SupportsSurface)
                {
                    m_PresentQueueIndex = i;
                    break;
                }
            }

            if (m_GraphicsQueueIndex == U32_MAX)
            {
                NOVA_VULKAN_ERROR("Failed to find a suitable graphics queue!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }

            if (m_PresentQueueIndex == U32_MAX)
            {
                NOVA_VULKAN_ERROR("Failed to find a suitable presentation queue!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }

            Array<VkDeviceQueueCreateInfo> QueueCreateInfos;
            const bool SameQueueFamilyIndices = m_GraphicsQueueIndex == m_PresentQueueIndex;
            static constexpr float QueuePriorities[] = {1.0f};
            if (SameQueueFamilyIndices)
            {
                const u32 QueueFamilyIndex = m_GraphicsQueueIndex;
                VkDeviceQueueCreateInfo QueueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
                QueueCreateInfo.queueCount = 1;
                QueueCreateInfo.pQueuePriorities = QueuePriorities;
                QueueCreateInfo.queueFamilyIndex = QueueFamilyIndex;
                QueueCreateInfos.Add(QueueCreateInfo);
            }
            else
            {
                VkDeviceQueueCreateInfo GraphicsQueueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
                GraphicsQueueCreateInfo.queueCount = 1;
                GraphicsQueueCreateInfo.pQueuePriorities = QueuePriorities;
                GraphicsQueueCreateInfo.queueFamilyIndex = m_GraphicsQueueIndex;
                QueueCreateInfos.Add(GraphicsQueueCreateInfo);

                VkDeviceQueueCreateInfo PresentQueueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
                PresentQueueCreateInfo.queueCount = 1;
                PresentQueueCreateInfo.pQueuePriorities = QueuePriorities;
                PresentQueueCreateInfo.queueFamilyIndex = m_PresentQueueIndex;
                QueueCreateInfos.Add(PresentQueueCreateInfo);
            }

            VkPhysicalDeviceDynamicRenderingFeatures DynamicRenderingFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES };
            DynamicRenderingFeatures.dynamicRendering = true;

            VkDeviceCreateInfo DeviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
            DeviceCreateInfo.pNext = &DynamicRenderingFeatures;
            DeviceCreateInfo.queueCreateInfoCount = (u32)QueueCreateInfos.Count();
            DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.Data();

            Array<const char*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, VK_EXT_SHADER_OBJECT_EXTENSION_NAME  };
            DeviceCreateInfo.enabledExtensionCount = (u32)DeviceExtensions.Count();
            DeviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.Data();

            VkPhysicalDeviceFeatures Features = {};
            Features.samplerAnisotropy = true;
            DeviceCreateInfo.pEnabledFeatures = &Features;

            if (VK_FAILED(vkCreateDevice(m_PhysicalDevice, &DeviceCreateInfo, nullptr, &m_Device)))
            {
                NOVA_VULKAN_ERROR("Failed to create device!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }


            VkDeviceQueueInfo2 QueueInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2 };
            QueueInfo.queueIndex = 0;
            QueueInfo.queueFamilyIndex = m_GraphicsQueueIndex;
            vkGetDeviceQueue2(m_Device, &QueueInfo, &m_GraphicsQueue);
            if (!m_GraphicsQueue)
            {
                NOVA_VULKAN_ERROR("Failed to retrieve graphics queue!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }

            QueueInfo.queueIndex = 0;
            QueueInfo.queueFamilyIndex = m_PresentQueueIndex;
            vkGetDeviceQueue2(m_Device, &QueueInfo, &m_PresentQueue);
            if (!m_PresentQueue)
            {
                NOVA_VULKAN_ERROR("Failed to retrieve present queue!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }
        }

        VkPhysicalDeviceSurfaceInfo2KHR SurfaceInfo = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR };
        SurfaceInfo.surface = m_Surface;

        u32 SurfaceFormatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &SurfaceFormatCount, nullptr);
        ScopedBuffer<VkSurfaceFormatKHR> SurfaceFormats(SurfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &SurfaceFormatCount, SurfaceFormats.GetData());


        //////////////////////////////////////////////////////////////////////////////////////////
        /// ALLOCATOR CREATE
        //////////////////////////////////////////////////////////////////////////////////////////
        {
            VmaAllocatorCreateInfo AllocatorCreateInfo = { 0 };
            AllocatorCreateInfo.device = m_Device;
            AllocatorCreateInfo.instance = m_Instance;
            AllocatorCreateInfo.physicalDevice = m_PhysicalDevice;
            AllocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_4;

            if (VK_FAILED(vmaCreateAllocator(&AllocatorCreateInfo, &m_Allocator)))
            {
                NOVA_VULKAN_ERROR("Failed to create allocator!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////////
        /// SWAPCHAIN CREATE
        //////////////////////////////////////////////////////////////////////////////////////////
        {
            const GraphicsSettings& GraphicsSettings = m_Application->GetGraphicsSettings();

            SwapchainCreateInfo CreateInfo;
            CreateInfo.Recycle = false;
            CreateInfo.ImageCount = (u32)GraphicsSettings.BufferType;
            CreateInfo.ImageFormat = Format::R8G8B8A8_UNORM;
            CreateInfo.ImageWidth = WindowWidth;
            CreateInfo.ImageHeight = WindowHeight;
            CreateInfo.ImagePresentMode = GetPresentMode();

            m_Swapchain = (VulkanSwapchain*)CreateSwapchain(CreateInfo);
            if (!m_Swapchain)
            {
                NOVA_VULKAN_ERROR("Failed to create swapchain!");
                return false;
            }

            CommandPoolCreateInfo CommandPoolCreateInfo;
            CommandPoolCreateInfo.Flags = CommandPoolFlagBits::ResetCommandBuffer;
            m_CommandPool = (VulkanCommandPool*)CreateCommandPool(CommandPoolCreateInfo);

            for (size_t i = 0; i < m_Swapchain->GetImageCount(); ++i)
            {
                m_Frames[i].SubmitSemaphore = (VulkanSemaphore*)CreateSemaphore({});
                m_Frames[i].SubmitSemaphore->SetDebugName("Submit Semaphore");
                m_Frames[i].PresentSemaphore = (VulkanSemaphore*)CreateSemaphore({});
                m_Frames[i].PresentSemaphore->SetDebugName("Present Semaphore");
                m_Frames[i].Fence = (VulkanFence*)CreateFence( { FenceCreateFlagBits::Signaled });

                VulkanCommandBuffer* Cmd = m_Frames[i].CommandBuffer = (VulkanCommandBuffer*)m_CommandPool->AllocateCommandBuffer({ CommandBufferLevel::Primary });
                Cmd->SetDebugName(StringFormat("Main CommandBuffer {}", i));
            }
        }

        {
            RenderTargetCreateInfo CreateInfo;
            CreateInfo.Width = m_Swapchain->GetWidth();
            CreateInfo.Height = m_Swapchain->GetHeight();
            CreateInfo.Depth = 1;
            CreateInfo.SampleCount = SampleCount::S8;
            CreateInfo.ColorFormat = Format::R8G8B8A8_UNORM;
            CreateInfo.DepthFormat = Format::D32_FLOAT_S8_UINT;
            m_RenderTarget = (VulkanRenderTarget*)CreateRenderTarget(CreateInfo);
            if (!m_RenderTarget)
            {
                NOVA_VULKAN_ERROR("Failed to create render target!");
                return false;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////////
        /// DESCRIPTOR POOL CREATE
        //////////////////////////////////////////////////////////////////////////////////////////
        {
            VkDescriptorPoolSize PoolSizes[] = {
                { VK_DESCRIPTOR_TYPE_SAMPLER, 32 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 32 },
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 32 }
            };
            VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
            DescriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            DescriptorPoolCreateInfo.maxSets = 2;
            DescriptorPoolCreateInfo.poolSizeCount = (u32)std::size(PoolSizes);
            DescriptorPoolCreateInfo.pPoolSizes = PoolSizes;

            if (VK_FAILED(vkCreateDescriptorPool(m_Device, &DescriptorPoolCreateInfo, nullptr, &m_DescriptorPool)))
            {
                NOVA_VULKAN_ERROR("Failed to create descriptor pool!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }
        }

        VkPhysicalDeviceProperties2 PhysicalDeviceProperties = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
        vkGetPhysicalDeviceProperties2(m_PhysicalDevice, &PhysicalDeviceProperties);
        char* PhysicalDeviceName = PhysicalDeviceProperties.properties.deviceName;
        const StringView GPUName(PhysicalDeviceName);

        NOVA_LOG(Application, Verbosity::Info, "Using Vulkan {}.{}", VK_API_VERSION_MAJOR(VK_API_VERSION_1_4),VK_API_VERSION_MINOR(VK_API_VERSION_1_4));
        NOVA_LOG(Application, Verbosity::Info, "GPU: {}", GPUName);
        return true;
    }


    void VulkanRenderer::Destroy()
    {
        vkDeviceWaitIdle(m_Device);

        vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);

        m_RenderTarget->Destroy();
        delete m_RenderTarget;

        for (size_t i = 0; i < m_Swapchain->GetImageCount(); ++i)
        {
            m_CommandPool->FreeCommandBuffer(m_Frames[i].CommandBuffer);
            m_Frames[i].Fence->Destroy();
            delete m_Frames[i].Fence;

            m_Frames[i].SubmitSemaphore->Destroy();
            delete m_Frames[i].SubmitSemaphore;

            m_Frames[i].PresentSemaphore->Destroy();
            delete m_Frames[i].PresentSemaphore;
        }

        vmaDestroyAllocator(m_Allocator);

        m_CommandPool->Destroy();
        delete m_CommandPool;
        m_Swapchain->Destroy();
        delete m_Swapchain;
        vkDestroyDevice(m_Device, nullptr);
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
#ifdef NOVA_DEBUG
        m_FunctionPointers.vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
#endif
        vkDestroyInstance(m_Instance, nullptr);
    }

    bool VulkanRenderer::BeginFrame()
    {
        const Window* Window = g_Application->GetWindow();
        const u32 WindowWidth = Window->GetWidth<u32>();
        const u32 WindowHeight = Window->GetHeight<u32>();

        if (Window->IsMinimized())
        {
            return false;
        }

        if (ShouldRecreateSwapchain)
        {
            WaitIdle();
            m_Swapchain->Recreate();
            m_RenderTarget->Resize(WindowWidth, WindowHeight);
            ShouldRecreateSwapchain = false;
            m_CurrentFrameIndex = 0;
            m_NewFrameIndex = 0;
            return false;
        }

        VulkanFence* Fence = m_Frames[m_CurrentFrameIndex].Fence;
        Fence->WaitForMe();
        Fence->Reset();

        m_NewFrameIndex = m_Swapchain->AcquireNextImage(m_Frames[m_CurrentFrameIndex].PresentSemaphore);
        VulkanCommandBuffer* CommandBuffer = m_Frames[m_CurrentFrameIndex].CommandBuffer;
        if (!CommandBuffer->Begin({ CommandBufferUsageFlagBits::OneTimeSubmit }))
            return false;

        return true;
    }

    void VulkanRenderer::BeginRendering()
    {
        VulkanCommandBuffer* CommandBuffer = GetCurrentCommandBuffer();
        m_RenderTarget->BeginRendering(CommandBuffer);
    }

    void VulkanRenderer::EndRendering()
    {
        VulkanCommandBuffer* CommandBuffer = GetCurrentCommandBuffer();
        m_RenderTarget->EndRendering(CommandBuffer);
        m_Swapchain->Resolve(CommandBuffer, m_RenderTarget);
    }

    void VulkanRenderer::EndFrame()
    {
        VulkanCommandBuffer* CommandBuffer = GetCurrentCommandBuffer();
        const VulkanFence* Fence = m_Frames[m_CurrentFrameIndex].Fence;
        CommandBuffer->End();

        const VkCommandBuffer SubmitCommandBuffers[1] { CommandBuffer->GetHandle() };
        VkSubmitInfo SubmitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        constexpr VkPipelineStageFlags Flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        SubmitInfo.pWaitDstStageMask = &Flags;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers = SubmitCommandBuffers;
        SubmitInfo.signalSemaphoreCount = 1;
        SubmitInfo.pSignalSemaphores = m_Frames[m_CurrentFrameIndex].SubmitSemaphore->GetHandlePtr();
        SubmitInfo.waitSemaphoreCount = 1;
        SubmitInfo.pWaitSemaphores = m_Frames[m_CurrentFrameIndex].PresentSemaphore->GetHandlePtr();

        if (VK_FAILED(vkQueueSubmit(m_GraphicsQueue, 1, &SubmitInfo, Fence->GetHandle())))
        {
            NOVA_VULKAN_ERROR("Failed to submit command buffer!");
            m_Application->RequireExit(ExitCode::Error);
        }
    }

    void VulkanRenderer::ClearDepth(const float Depth)
    {
        const u32 WindowWidth = g_Application->GetWindow()->GetWidth<u32>();
        const u32 WindowHeight = g_Application->GetWindow()->GetHeight<u32>();

        const VkCommandBuffer CommandBuffer = GetCurrentCommandBuffer()->GetHandle();
        VkClearAttachment ClearAttachment;
        ClearAttachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        ClearAttachment.clearValue.depthStencil = VkClearDepthStencilValue{ Depth, 0 };

        VkClearRect ClearRect;
        ClearRect.rect.offset = VkOffset2D{ 0, 0 };
        ClearRect.rect.extent = VkExtent2D{ WindowWidth, WindowHeight };
        ClearRect.layerCount = 1;
        ClearRect.baseArrayLayer = 0;

        vkCmdClearAttachments(CommandBuffer, 1, &ClearAttachment, 1u, &ClearRect);
    }

    void VulkanRenderer::ClearColor(const Color& color)
    {
        const u32 WindowWidth = g_Application->GetWindow()->GetWidth<u32>();
        const u32 WindowHeight = g_Application->GetWindow()->GetHeight<u32>();

        const VkCommandBuffer CommandBuffer = GetCurrentCommandBuffer()->GetHandle();
        VkClearAttachment ClearAttachment;
        ClearAttachment.colorAttachment = 0;
        ClearAttachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ClearAttachment.clearValue.color = VkClearColorValue{ { color.r, color.g, color.b, color.a }};

        VkClearRect ClearRect;
        ClearRect.rect.extent = VkExtent2D{ WindowWidth, WindowHeight };
        ClearRect.rect.offset = VkOffset2D{ 0, 0 };
        ClearRect.layerCount = 1;
        ClearRect.baseArrayLayer = 0;

        vkCmdClearAttachments(CommandBuffer, 1, &ClearAttachment, 1u, &ClearRect);
    }

    void VulkanRenderer::Clear(const Color& Color, const float Depth)
    {
        const u32 WindowWidth = g_Application->GetWindow()->GetWidth<u32>();
        const u32 WindowHeight = g_Application->GetWindow()->GetHeight<u32>();

        const VkCommandBuffer Cmd = GetCurrentCommandBuffer()->GetHandle();
        VkClearAttachment ClearColorAttachment;
        ClearColorAttachment.colorAttachment = 0;
        ClearColorAttachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ClearColorAttachment.clearValue.color = VkClearColorValue{ { Color.r, Color.g, Color.b, Color.a }};

        VkClearAttachment ClearDepthAttachment;
        ClearDepthAttachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        ClearDepthAttachment.clearValue.depthStencil = VkClearDepthStencilValue{ Depth, 0 };

        const VkClearAttachment ClearAttachments[] = { ClearColorAttachment, ClearDepthAttachment };


        VkClearRect ClearRect;
        ClearRect.rect.extent = VkExtent2D{ WindowWidth, WindowHeight };
        ClearRect.rect.offset = VkOffset2D{ 0, 0 };
        ClearRect.layerCount = 1;
        ClearRect.baseArrayLayer = 0;

        vkCmdClearAttachments(Cmd, std::size(ClearAttachments), ClearAttachments, 1u, &ClearRect);
    }

    void VulkanRenderer::Present()
    {
        if (!m_Swapchain)
            return;

        VkPresentInfoKHR PresentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        PresentInfo.swapchainCount = 1;
        PresentInfo.pSwapchains = m_Swapchain->GetHandlePtr();
        PresentInfo.pImageIndices = &m_NewFrameIndex;
        PresentInfo.waitSemaphoreCount = 1;
        PresentInfo.pWaitSemaphores = m_Frames[m_CurrentFrameIndex].SubmitSemaphore->GetHandlePtr();

        if (VK_FAILED(vkQueuePresentKHR(m_GraphicsQueue, &PresentInfo)))
        {
            NOVA_VULKAN_ERROR("Failed to present!");
            m_Application->RequireExit(ExitCode::Error);
        }

        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_Swapchain->GetImageCount();
    }


    void VulkanRenderer::SetViewport(const Viewport& Viewport)
    {
        const VkCommandBuffer Cmd = GetCurrentCommandBuffer()->GetHandle();
        VkViewport Result { };
        Result.x = Viewport.X;
        Result.y = Viewport.Y + Viewport.Height;
        Result.width = Viewport.Width;
        Result.height = -Viewport.Height;
        Result.minDepth = Viewport.MinDepth;
        Result.maxDepth = Viewport.MaxDepth;
        vkCmdSetViewport(Cmd, 0, 1, &Result);
    }

    void VulkanRenderer::SetScissor(const Scissor& Scissor)
    {
        const VkCommandBuffer Cmd = GetCurrentCommandBuffer()->GetHandle();
        VkRect2D Rect { };
        Rect.offset.x = Scissor.X;
        Rect.offset.y = Scissor.Y;
        Rect.extent.width = Scissor.Width;
        Rect.extent.height = Scissor.Height;
        vkCmdSetScissor(Cmd, 0, 1, &Rect);
    }


    void VulkanRenderer::DrawIndexed(const size_t IndexCount, const size_t Offset)
    {
        const VkCommandBuffer Cmd = GetCurrentCommandBuffer()->GetHandle();
        vkCmdDrawIndexed(Cmd, IndexCount, 1, 0, Offset, 0);
    }


    void VulkanRenderer::SetBlending(const bool Enabled)
    {
        const VkCommandBuffer Cmd = GetCurrentCommandBuffer()->GetHandle();
        const VkBool32 Enables[] = { Enabled };
        m_FunctionPointers.vkCmdSetColorBlendEnableEXT(Cmd, 0, 1, Enables);
    }

    void VulkanRenderer::SetCullMode(const CullMode Mode)
    {
        const VkCommandBuffer Cmd = GetCurrentCommandBuffer()->GetHandle();
        vkCmdSetCullMode(Cmd, Convertor.ConvertCullMode(Mode));
    }

    void VulkanRenderer::SetDepthCompareOperation(const CompareOperation DepthFunction)
    {
        const VkCommandBuffer Cmd = GetCurrentCommandBuffer()->GetHandle();
        vkCmdSetDepthCompareOp(Cmd, Convertor.ConvertCompareOperation(DepthFunction));
    }

    void VulkanRenderer::SetBlendFunction(const BlendFactor ColorSource, const BlendFactor ColorDest, const BlendOperation ColorOperation, const BlendFactor AlphaSource, const BlendFactor AlphaDest, const BlendOperation AlphaOperation)
    {
        const VkCommandBuffer Cmd = GetCurrentCommandBuffer()->GetHandle();
        const VkColorBlendEquationEXT ColorBlendEquation {
            Convertor.ConvertBlendFactor(ColorSource), Convertor.ConvertBlendFactor(ColorDest), Convertor.ConvertBlendOperation(ColorOperation),
            Convertor.ConvertBlendFactor(AlphaSource), Convertor.ConvertBlendFactor(AlphaDest), Convertor.ConvertBlendOperation(AlphaOperation)
        };
        m_FunctionPointers.vkCmdSetColorBlendEquationEXT(Cmd, 0, 1, &ColorBlendEquation);
    }

    void VulkanRenderer::SetBlendFunction(const BlendFactor Source, const BlendFactor Destination, const BlendOperation Operation)
    {
        SetBlendFunction(Source, Destination, Operation, Source, Destination, Operation);
    }

    void VulkanRenderer::BindPipeline(Pipeline* Pipeline)
    {
        const VulkanPipeline* CastedPipeline = Pipeline->As<VulkanPipeline>();
        const PipelineSpecification& Specification = CastedPipeline->GetSpecification();
        const VulkanShader* Shader = Specification.ShaderProgram->As<VulkanShader>();
        const VkCommandBuffer Cmd = GetCurrentCommandBuffer()->GetHandle();
        vkCmdBindPipeline(Cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, CastedPipeline->GetHandle());

        const Array<VkDescriptorSet>& DescriptorSets = Shader->GetDescriptorSets();
        if (DescriptorSets.Count() > 0)
        {
            vkCmdBindDescriptorSets(Cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, Shader->GetPipelineLayout(), 0, DescriptorSets.Count(), DescriptorSets.Data(), 0, nullptr);
        }
    }

    void VulkanRenderer::UpdateUniformBuffer(UniformBuffer* Buffer, const u64 Offset, const u64 Size, const void* Data)
    {
        const VkCommandBuffer Cmd = GetCurrentCommandBuffer()->GetHandle();
        vkCmdUpdateBuffer(Cmd, Buffer->As<VulkanUniformBuffer>()->GetHandle(), Offset, Size, Data);
    }

    PresentMode VulkanRenderer::GetPresentMode() const
    {
        const GraphicsSettings& GraphicsSettings = m_Application->GetGraphicsSettings();
        VkPresentModeKHR Result;

        if (GraphicsSettings.VSync)
        {
            Result = VK_PRESENT_MODE_FIFO_KHR;

            u32 PresentModeCount;
            VkPresentModeKHR PresentModes[8];
            vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &PresentModeCount, PresentModes);

            for (size_t PresentModeIndex = 0; PresentModeIndex < PresentModeCount; ++PresentModeIndex)
            {
                const VkPresentModeKHR PresentMode = PresentModes[PresentModeIndex];
                if (PresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    Result = PresentMode;
                    break;
                }
            }
        } else
        {
            Result = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }

        switch (Result) {
        case VK_PRESENT_MODE_IMMEDIATE_KHR: return PresentMode::Immediate;
        case VK_PRESENT_MODE_MAILBOX_KHR: return PresentMode::Mailbox;
        case VK_PRESENT_MODE_FIFO_KHR: return PresentMode::Fifo;
        default: throw;
        }
    }

    void VulkanRenderer::BindVertexBuffer(VertexBuffer* Buffer, const u64 Offset)
    {
        const VkCommandBuffer Cmd = GetCurrentCommandBuffer()->GetHandle();
        vkCmdBindVertexBuffers(Cmd, 0, 1, Buffer->As<VulkanVertexBuffer>()->GetHandlePtr(), &Offset);
    }

    void VulkanRenderer::BindIndexBuffer(IndexBuffer* Buffer, const u64 Offset)
    {
        const VkCommandBuffer Cmd = GetCurrentCommandBuffer()->GetHandle();
        vkCmdBindIndexBuffer(Cmd, Buffer->As<VulkanIndexBuffer>()->GetHandle(), Offset, VK_INDEX_TYPE_UINT32);
    }

    VkInstance VulkanRenderer::GetInstance() const
    {
        return m_Instance;
    }

    VkPhysicalDevice VulkanRenderer::GetPhysicalDevice() const
    {
        return m_PhysicalDevice;
    }

    VkDevice VulkanRenderer::GetDevice() const
    {
        return m_Device;
    }

    VkQueue VulkanRenderer::GetGraphicsQueue() const
    {
        return m_GraphicsQueue;
    }

    VkQueue VulkanRenderer::GetPresentQueue() const
    {
        return m_PresentQueue;
    }

    VkSurfaceKHR VulkanRenderer::GetSurface() const
    {
        return m_Surface;
    }

    VulkanSwapchain* VulkanRenderer::GetSwapchain() const
    {
        return m_Swapchain;
    }

    u32 VulkanRenderer::GetGraphicsQueueFamily() const
    {
        return m_GraphicsQueueIndex;
    }

    u32 VulkanRenderer::GetPresentQueueFamily() const
    {
        return m_PresentQueueIndex;
    }

    VmaAllocator VulkanRenderer::GetAllocator() const
    {
        return m_Allocator;
    }

    BufferView<VulkanFrameData> VulkanRenderer::GetFrameData() const
    {
        return { m_Frames, (size_t)m_Swapchain->GetImageCount() };
    }

    VulkanFrameData VulkanRenderer::GetCurrentFrameData() const
    {
        return m_Frames[m_CurrentFrameIndex];
    }

    VulkanCommandBuffer* VulkanRenderer::GetCurrentCommandBuffer() const
    {
        return m_Frames[m_CurrentFrameIndex].CommandBuffer;
    }

    u32 VulkanRenderer::GetImageCount() const
    {
        return m_Swapchain->GetImageCount();
    }

    VkDescriptorPool VulkanRenderer::GetDescriptorPool() const
    {
        return m_DescriptorPool;
    }

    VulkanCommandPool* VulkanRenderer::GetCommandPool() const
    {
        return m_CommandPool;
    }

    void VulkanRenderer::WaitIdle() const
    {
        vkDeviceWaitIdle(m_Device);
    }

    const VkFunctionPointers& VulkanRenderer::GetFunctionPointers() const
    {
        return m_FunctionPointers;
    }

    u32 VulkanRenderer::GetCurrentFrameIndex() const
    {
        return m_CurrentFrameIndex;
    }


}
