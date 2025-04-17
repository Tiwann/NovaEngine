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

#include "VulkanPipeline.h"

namespace Nova
{
    #if defined(NOVA_DEBUG)
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
        m_ImageCount = (u32)g_Application->GetConfiguration().Graphics.BufferType;
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

            #if defined(NOVA_DEBUG)
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

#if defined(NOVA_DEBUG)
            m_FunctionPointers.vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
            m_FunctionPointers.vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
#endif
            m_FunctionPointers.vkCmdSetColorBlendEnableEXT = (PFN_vkCmdSetColorBlendEnableEXT)vkGetInstanceProcAddr(m_Instance, "vkCmdSetColorBlendEnableEXT");
            m_FunctionPointers.vkCmdSetColorBlendEquationEXT = (PFN_vkCmdSetColorBlendEquationEXT)vkGetInstanceProcAddr(m_Instance, "vkCmdSetColorBlendEquationEXT");
            m_FunctionPointers.vkCreateShadersEXT = (PFN_vkCreateShadersEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateShadersEXT");
            m_FunctionPointers.vkDestroyShaderEXT = (PFN_vkDestroyShaderEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyShaderEXT");
            m_FunctionPointers.vkCmdBindShadersEXT = (PFN_vkCmdBindShadersEXT)vkGetInstanceProcAddr(m_Instance, "vkCmdBindShadersEXT");
        }

        //////////////////////////////////////////////////////////////////////////////////////////
        /// DEBUG MESSENGER CREATE
        //////////////////////////////////////////////////////////////////////////////////////////
        #if defined(NOVA_DEBUG)
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

                ScopedPointer<PopupMessage> Message = PopupMessage::
                Create("Fatal Error", "No suitable GPU found!", PopupMessageResponse::OK, PopupMessageIcon::Error);
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


            VkPhysicalDeviceShaderObjectFeaturesEXT ShaderObjectFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT };
            ShaderObjectFeatures.shaderObject = true;

            VkPhysicalDeviceDynamicRenderingFeatures DynamicRenderingFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES };
            DynamicRenderingFeatures.pNext = &ShaderObjectFeatures;
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


        const GraphicsSettings& GraphicsSettings = m_Application->GetGraphicsSettings();
        if (GraphicsSettings.VSync)
        {
            m_PresentMode = VK_PRESENT_MODE_FIFO_KHR;

            u32 PresentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &PresentModeCount, nullptr);
            ScopedBuffer<VkPresentModeKHR> PresentModes(PresentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &SurfaceFormatCount, PresentModes.GetData());
            for (const VkPresentModeKHR PresentMode : PresentModes)
            {
                if (PresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    m_PresentMode = PresentMode;
                    break;
                }
            }
        } else
        {
            m_PresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }

        //////////////////////////////////////////////////////////////////////////////////////////
        /// SAWPCHAIN POOL CREATE
        //////////////////////////////////////////////////////////////////////////////////////////
        {
            VkSwapchainCreateInfoKHR SwapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
            SwapchainCreateInfo.surface = m_Surface;
            SwapchainCreateInfo.imageFormat = m_SwapchainImageFormat;
            SwapchainCreateInfo.imageColorSpace = m_ImageColorSpace = SurfaceFormats[0].colorSpace;
            SwapchainCreateInfo.presentMode = m_PresentMode;
            SwapchainCreateInfo.clipped = true;
            SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            SwapchainCreateInfo.imageExtent.width = WindowWidth;
            SwapchainCreateInfo.imageExtent.height = WindowHeight;
            SwapchainCreateInfo.minImageCount = m_ImageCount;
            SwapchainCreateInfo.imageArrayLayers = 1;
            SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            SwapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
            SwapchainCreateInfo.oldSwapchain = nullptr;

            if (m_GraphicsQueue == m_PresentQueue)
            {
                SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                SwapchainCreateInfo.queueFamilyIndexCount = 1;
                SwapchainCreateInfo.pQueueFamilyIndices = &m_GraphicsQueueIndex;
            }
            else
            {
                const u32 QueueFamilyIndices[2] = { m_GraphicsQueueIndex, m_PresentQueueIndex };
                SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                SwapchainCreateInfo.pQueueFamilyIndices = QueueFamilyIndices;
            }

            if (VK_FAILED(vkCreateSwapchainKHR(m_Device, &SwapchainCreateInfo, nullptr, &m_Swapchain)))
            {
                NOVA_VULKAN_ERROR("Failed to create swapchain!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }

            u32 SwapchainImageCount;
            vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &SwapchainImageCount, nullptr);
            ScopedBuffer<VkImage> SwapchainImages(SwapchainImageCount);
            if (VK_FAILED(vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &SwapchainImageCount, SwapchainImages.GetData())))
            {
                NOVA_VULKAN_ERROR("Failed to get swapchain image!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }

            VkCommandPoolCreateInfo CommandPoolCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
            CommandPoolCreateInfo.queueFamilyIndex = m_GraphicsQueueIndex;
            CommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            if (VK_FAILED(vkCreateCommandPool(m_Device, &CommandPoolCreateInfo, nullptr, &m_CommandPool)))
            {
                NOVA_VULKAN_ERROR("Failed to create command pool!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }

            for (size_t i = 0; i < m_ImageCount; i++)
            {
                const VkImage Image = SwapchainImages[i];

                VkImageViewCreateInfo ImageViewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
                ImageViewCreateInfo.image = Image;
                ImageViewCreateInfo.components = VkComponentMapping();
                ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                ImageViewCreateInfo.format = m_SwapchainImageFormat;
                ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
                ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
                ImageViewCreateInfo.subresourceRange.levelCount = 1;
                ImageViewCreateInfo.subresourceRange.layerCount = 1;

                if (VK_FAILED(vkCreateImageView(m_Device, &ImageViewCreateInfo, nullptr, &m_Frames[i].ImageView)))
                {
                    NOVA_VULKAN_ERROR("Failed to create image view!");
                    m_Application->RequireExit(ExitCode::Error);
                    return false;
                }

                m_Frames[i].Image = Image;

                VkSemaphoreCreateInfo SemaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
                if (VK_FAILED(vkCreateSemaphore(m_Device, &SemaphoreCreateInfo, nullptr, &m_Frames[i].SubmitSemaphore)))
                {
                    NOVA_VULKAN_ERROR("Failed to create Submit Semaphore! (Image {})", i);
                    return false;
                }

                if (VK_FAILED(vkCreateSemaphore(m_Device, &SemaphoreCreateInfo, nullptr, &m_Frames[i].PresentSemaphore)))
                {
                    NOVA_VULKAN_ERROR("Failed to create Present Semaphore! (Image {})", i);
                    return false;
                }

                VkFenceCreateInfo FenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
                FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
                if (VK_FAILED(vkCreateFence(m_Device, &FenceCreateInfo, nullptr, &m_Frames[i].Fence)))
                {
                    NOVA_VULKAN_ERROR("Failed to create Fence!");
                    m_Application->RequireExit(ExitCode::Error);
                    return false;
                }

                // Allocate command buffers
                {
                    VkCommandBufferAllocateInfo CommandBufferAllocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
                    CommandBufferAllocateInfo.commandPool = m_CommandPool;
                    CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                    CommandBufferAllocateInfo.commandBufferCount = 1;

                    if (VK_FAILED(vkAllocateCommandBuffers(m_Device, &CommandBufferAllocateInfo, &m_Frames[i].CommandBuffer)))
                    {
                        NOVA_VULKAN_ERROR("Failed to allocate command buffer (Image {})!", i);
                        m_Application->RequireExit(ExitCode::Error);
                        return false;
                    }
                }
            }
        }

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
        /// DESCRIPTOR POOL CREATE
        //////////////////////////////////////////////////////////////////////////////////////////
        {
            VkDescriptorPoolSize PoolSizes[] = {
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 8 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 16 },
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 64 }
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

        //////////////////////////////////////////////////////////////////////////////////////////
        /// GRAPHICS PIPELINE CREATE
        //////////////////////////////////////////////////////////////////////////////////////////
        /*{

            Array<VkVertexInputAttributeDescription> VertexInputAttributeDescriptions
            {
                VkVertexInputAttributeDescription{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Position) },
                VkVertexInputAttributeDescription{ 0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, TextureCoordinate) },
                VkVertexInputAttributeDescription{ 0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Normal) },
                VkVertexInputAttributeDescription{ 0, 3, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, Color) },
            };

            Array<VkVertexInputBindingDescription> VertexInputBindingDescriptions
            {
                VkVertexInputBindingDescription {  0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX },
                VkVertexInputBindingDescription {  1, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX },
                VkVertexInputBindingDescription {  2, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX },
                VkVertexInputBindingDescription {  3, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX }
            };

            // Configuration du pipeline
            VkPipelineVertexInputStateCreateInfo VertexInputInfo{};
            VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            VertexInputInfo.vertexAttributeDescriptionCount = VertexInputAttributeDescriptions.Count();
            VertexInputInfo.pVertexAttributeDescriptions = VertexInputAttributeDescriptions.Data();
            VertexInputInfo.vertexBindingDescriptionCount = VertexInputBindingDescriptions.Count();
            VertexInputInfo.pVertexBindingDescriptions = VertexInputBindingDescriptions.Data();

            VkPipelineInputAssemblyStateCreateInfo InputAssembly{};
            InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            InputAssembly.primitiveRestartEnable = VK_FALSE;

            VkPipelineRasterizationStateCreateInfo Rasterizer{};
            Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            Rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            Rasterizer.lineWidth = 1.0f;

            VkPipelineMultisampleStateCreateInfo Multisampling{};
            Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

            VkPipelineColorBlendAttachmentState ColorBlendAttachment{};
            ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            ColorBlendAttachment.blendEnable = VK_FALSE;

            VkPipelineColorBlendStateCreateInfo ColorBlending{};
            ColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            ColorBlending.attachmentCount = 1;
            ColorBlending.pAttachments = &ColorBlendAttachment;

            VkPipelineRenderingCreateInfoKHR PipelineRenderingInfo{};
            PipelineRenderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
            PipelineRenderingInfo.colorAttachmentCount = 1;
            PipelineRenderingInfo.pColorAttachmentFormats = &m_SwapchainImageFormat;


            VkPipelineShaderStageCreateInfo VertexStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
            VertexStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;


            VkGraphicsPipelineCreateInfo PipelineInfo{};
            PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            PipelineInfo.pVertexInputState = &VertexInputInfo;
            PipelineInfo.pInputAssemblyState = &InputAssembly;
            PipelineInfo.pRasterizationState = &Rasterizer;
            PipelineInfo.pMultisampleState = &Multisampling;
            PipelineInfo.pColorBlendState = &ColorBlending;
            PipelineInfo.layout = m_PipelineLayout;
            PipelineInfo.pNext = &PipelineRenderingInfo;
            PipelineInfo.stageCount = 2;
            PipelineInfo.pStages

            if (vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
            {
                NOVA_VULKAN_ERROR("Failed to create graphics pipeline!");
                return false;
            }

        }*/

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
        vmaDestroyAllocator(m_Allocator);

        for (size_t i = 0; i < m_ImageCount; ++i)
        {
            vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &m_Frames[i].CommandBuffer);
            vkDestroyFence(m_Device, m_Frames[i].Fence, nullptr);
            vkDestroySemaphore(m_Device, m_Frames[i].SubmitSemaphore, nullptr);
            vkDestroySemaphore(m_Device, m_Frames[i].PresentSemaphore, nullptr);
            vkDestroyImageView(m_Device, m_Frames[i].ImageView, nullptr);
        }
        vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
        vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
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
            vkDeviceWaitIdle(m_Device);

            for (size_t i = 0; i < m_ImageCount; ++i)
            {
                VkFrameData& VkFrameData = m_Frames[i];
                vkDestroyImageView(m_Device, VkFrameData.ImageView, nullptr);
            }

            VkSwapchainCreateInfoKHR SwapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
            SwapchainCreateInfo.surface = m_Surface;
            SwapchainCreateInfo.imageFormat = m_SwapchainImageFormat;
            SwapchainCreateInfo.imageColorSpace = m_ImageColorSpace;
            SwapchainCreateInfo.presentMode = m_PresentMode;
            SwapchainCreateInfo.clipped = true;
            SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            SwapchainCreateInfo.imageExtent = VkExtent2D(WindowWidth, WindowHeight);
            SwapchainCreateInfo.minImageCount = m_ImageCount;
            SwapchainCreateInfo.imageArrayLayers = 1;
            SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            SwapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
            SwapchainCreateInfo.oldSwapchain = m_Swapchain;

            if (m_GraphicsQueue == m_PresentQueue)
            {
                SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                SwapchainCreateInfo.queueFamilyIndexCount = 1;
                SwapchainCreateInfo.pQueueFamilyIndices = &m_GraphicsQueueIndex;
            }
            else
            {
                const u32 QueueFamilyIndices[2] = { m_GraphicsQueueIndex, m_PresentQueueIndex };
                SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                SwapchainCreateInfo.pQueueFamilyIndices = QueueFamilyIndices;
            }

            VkSwapchainKHR NewSwapchain = nullptr;
            if (VK_FAILED(vkCreateSwapchainKHR(m_Device, &SwapchainCreateInfo, nullptr, &NewSwapchain)))
            {
                NOVA_VULKAN_ERROR("Failed to create swapchain!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }

            vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
            m_Swapchain = NewSwapchain;

            u32 SwapchainImageCount;
            vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &SwapchainImageCount, nullptr);
            ScopedBuffer<VkImage> SwapchainImages(SwapchainImageCount);
            if (VK_FAILED(vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &SwapchainImageCount, SwapchainImages.GetData())))
            {
                NOVA_VULKAN_ERROR("Failed to get swapchain image!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }

            for (size_t i = 0; i < m_ImageCount; i++)
            {
                 m_Frames[i].Image = SwapchainImages[i];

                VkImageViewCreateInfo ImageViewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
                ImageViewCreateInfo.image =  m_Frames[i].Image;
                ImageViewCreateInfo.components = VkComponentMapping();
                ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                ImageViewCreateInfo.format = m_SwapchainImageFormat;
                ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
                ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
                ImageViewCreateInfo.subresourceRange.levelCount = 1;
                ImageViewCreateInfo.subresourceRange.layerCount = 1;

                if (VK_FAILED(vkCreateImageView(m_Device, &ImageViewCreateInfo, nullptr, &m_Frames[i].ImageView)))
                {
                    NOVA_VULKAN_ERROR("Failed to create image view!");
                    m_Application->RequireExit(ExitCode::Error);
                    return false;
                }

                // Allocate command buffers
                {
                    VkCommandBufferAllocateInfo CommandBufferAllocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
                    CommandBufferAllocateInfo.commandPool = m_CommandPool;
                    CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                    CommandBufferAllocateInfo.commandBufferCount = 1;

                    if (VK_FAILED(vkAllocateCommandBuffers(m_Device, &CommandBufferAllocateInfo, &m_Frames[i].CommandBuffer)))
                    {
                        NOVA_VULKAN_ERROR("Failed to allocate command buffer (Image {})!", i);
                        m_Application->RequireExit(ExitCode::Error);
                        return false;
                    }
                }
            }

            ShouldRecreateSwapchain = false;
            m_CurrentFrameIndex = 0;
            m_NewFrameIndex = 0;
            return false;
        }

        const VkFence& CurrentFence = m_Frames[m_CurrentFrameIndex].Fence;
        if (VK_FAILED(vkWaitForFences(m_Device, 1, &CurrentFence, true, U64_MAX)))
        {
            NOVA_VULKAN_ERROR("Failed to wait for fence {}!", m_CurrentFrameIndex);
            return false;
        }

        if (VK_FAILED(vkResetFences(m_Device, 1, &CurrentFence)))
        {
            NOVA_VULKAN_ERROR("Failed to reset fence {}!", m_CurrentFrameIndex);
            return false;
        }


        if (VK_FAILED(vkAcquireNextImageKHR(m_Device, m_Swapchain, U64_MAX, m_Frames[m_CurrentFrameIndex].PresentSemaphore, nullptr, &m_NewFrameIndex)))
        {
            NOVA_VULKAN_ERROR("Failed to acquire next image index!");
            return false;
        }

        VkCommandBufferBeginInfo BeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        const VkCommandBuffer CommandBuffer = m_Frames[m_CurrentFrameIndex].CommandBuffer;
        if (VK_FAILED(vkBeginCommandBuffer(CommandBuffer, &BeginInfo)))
        {
            NOVA_VULKAN_ERROR("Failed to begin command buffer!");
            return false;
        }


        VkImageMemoryBarrier Barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        Barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        Barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        Barrier.image = m_Frames[m_CurrentFrameIndex].Image;
        Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        Barrier.subresourceRange.baseMipLevel = 0;
        Barrier.subresourceRange.levelCount = 1;
        Barrier.subresourceRange.baseArrayLayer = 0;
        Barrier.subresourceRange.layerCount = 1;
        Barrier.srcAccessMask = VK_ACCESS_NONE_KHR;
        Barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        vkCmdPipelineBarrier(
            CommandBuffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0, 0, nullptr, 0, nullptr, 1, &Barrier
        );

        VkRenderingAttachmentInfo RenderingAttachmentInfo = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        RenderingAttachmentInfo.clearValue.color = VkClearColorValue{ { 0.0f, 0.0, 0.0, 1.0f } };
        RenderingAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        RenderingAttachmentInfo.imageView = m_Frames[m_CurrentFrameIndex].ImageView;
        RenderingAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        RenderingAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingInfo RenderingInfo = { VK_STRUCTURE_TYPE_RENDERING_INFO };
        RenderingInfo.layerCount = 1;
        RenderingInfo.renderArea.extent = VkExtent2D{ WindowWidth, WindowHeight };
        RenderingInfo.renderArea.offset = VkOffset2D{ 0, 0 };
        RenderingInfo.viewMask = 0;
        RenderingInfo.colorAttachmentCount = 1;
        RenderingInfo.pColorAttachments = &RenderingAttachmentInfo;

        vkCmdBeginRendering(CommandBuffer, &RenderingInfo);

        return true;
    }

    void VulkanRenderer::EndFrame()
    {
        const VkCommandBuffer& CommandBuffer = m_Frames[m_CurrentFrameIndex].CommandBuffer;
        const VkFence& Fence = m_Frames[m_CurrentFrameIndex].Fence;

        vkCmdEndRendering(CommandBuffer);

        VkImageMemoryBarrier Barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        Barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        Barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        Barrier.image = m_Frames[m_CurrentFrameIndex].Image;
        Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        Barrier.subresourceRange.baseMipLevel = 0;
        Barrier.subresourceRange.levelCount = 1;
        Barrier.subresourceRange.baseArrayLayer = 0;
        Barrier.subresourceRange.layerCount = 1;
        Barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        Barrier.dstAccessMask = VK_ACCESS_NONE_KHR;

        vkCmdPipelineBarrier(
            CommandBuffer,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0, 0, nullptr, 0, nullptr, 1, &Barrier
        );
        VK_CHECK_MSG(vkEndCommandBuffer(CommandBuffer), "Failed to end command buffer!");

        VkSubmitInfo SubmitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        constexpr VkPipelineStageFlags Flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        SubmitInfo.pWaitDstStageMask = &Flags;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers = &CommandBuffer;
        SubmitInfo.signalSemaphoreCount = 1;
        SubmitInfo.pSignalSemaphores = &m_Frames[m_CurrentFrameIndex].SubmitSemaphore;
        SubmitInfo.waitSemaphoreCount = 1;
        SubmitInfo.pWaitSemaphores = &m_Frames[m_CurrentFrameIndex].PresentSemaphore;


        if (VK_FAILED(vkQueueSubmit(m_GraphicsQueue, 1, &SubmitInfo, Fence)))
        {
            NOVA_VULKAN_ERROR("Failed to submit command buffer!");
            m_Application->RequireExit(ExitCode::Error);
        }
    }

    void VulkanRenderer::ClearDepth(const float Depth)
    {
        const u32 WindowWidth = g_Application->GetWindow()->GetWidth<u32>();
        const u32 WindowHeight = g_Application->GetWindow()->GetHeight<u32>();

        const VkCommandBuffer CommandBuffer = m_Frames[m_CurrentFrameIndex].CommandBuffer;
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

        const VkCommandBuffer CommandBuffer = m_Frames[m_CurrentFrameIndex].CommandBuffer;
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

        const VkCommandBuffer Cmd = GetCurrentCommandBuffer();
        VkClearAttachment ClearColorAttachment;
        ClearColorAttachment.colorAttachment = 0;
        ClearColorAttachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ClearColorAttachment.clearValue.color = VkClearColorValue{ { Color.r, Color.g, Color.b, Color.a }};

        VkClearAttachment ClearDepthAttachment;
        ClearDepthAttachment.colorAttachment = 0;
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
        VkPresentInfoKHR PresentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        PresentInfo.swapchainCount = 1;
        PresentInfo.pSwapchains = &m_Swapchain;
        PresentInfo.pImageIndices = &m_NewFrameIndex;
        PresentInfo.waitSemaphoreCount = 1;
        PresentInfo.pWaitSemaphores = &m_Frames[m_CurrentFrameIndex].SubmitSemaphore;

        if (VK_FAILED(vkQueuePresentKHR(m_GraphicsQueue, &PresentInfo)))
        {
            NOVA_VULKAN_ERROR("Failed to present!");
            m_Application->RequireExit(ExitCode::Error);
        }

        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_ImageCount;
    }


    void VulkanRenderer::SetViewportRect(const Vector2 Position, const Vector2 Size)
    {
        const VkCommandBuffer Cmd = GetCurrentCommandBuffer();
        const VkViewport Viewport = { Position.x, Position.y, Size.x, Size.y, 0.0f, 1.0f };
        vkCmdSetViewport(Cmd, 0, 1, &Viewport);
    }

    void VulkanRenderer::Draw(VertexArray* VAO, const u32 NumVert, Shader* Shader)
    {
        const VkCommandBuffer Cmd = GetCurrentCommandBuffer();
        vkCmdDraw(Cmd, NumVert, 1, 0, 0);
    }

    void VulkanRenderer::DrawIndexed(VertexArray* VertexArray, VertexBuffer* VertexBuffer, IndexBuffer* IndexBuffer, Shader* Shader)
    {
        const VkCommandBuffer Cmd = GetCurrentCommandBuffer();
        Array<VkVertexInputAttributeDescription2EXT> VertexInputAttributeDescriptions
            {
                VkVertexInputAttributeDescription2EXT{ VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT, nullptr, 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Position) },
                VkVertexInputAttributeDescription2EXT{ VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT, nullptr, 1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, TextureCoordinate) },
                VkVertexInputAttributeDescription2EXT{ VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT, nullptr, 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Normal) },
                VkVertexInputAttributeDescription2EXT{ VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT, nullptr, 3, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, Color) },
            };

        Array<VkVertexInputBindingDescription2EXT> VertexInputBindingDescriptions
        {
            VkVertexInputBindingDescription2EXT { VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT, nullptr, 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX, 1 }
        };

        const PFN_vkCmdSetVertexInputEXT vkCmdSetVertexInputEXT = (PFN_vkCmdSetVertexInputEXT)vkGetInstanceProcAddr(m_Instance, "vkCmdSetVertexInputEXT");
        vkCmdSetVertexInputEXT(Cmd, VertexInputBindingDescriptions.Count(), VertexInputBindingDescriptions.Data(), VertexInputAttributeDescriptions.Count(), VertexInputAttributeDescriptions.Data());

        const PFN_vkCmdSetRasterizerDiscardEnable vkCmdSetRasterizerDiscardEnable = (PFN_vkCmdSetRasterizerDiscardEnable)vkGetInstanceProcAddr(m_Instance, "vkCmdSetRasterizerDiscardEnable");
        vkCmdSetRasterizerDiscardEnable(GetCurrentCommandBuffer(), false);

        SetCullMode(CullMode::BackFace);
        vkCmdSetFrontFace(GetCurrentCommandBuffer(), VK_FRONT_FACE_CLOCKWISE);

        const PFN_vkCmdSetDepthTestEnable vkCmdSetDepthTestEnable = (PFN_vkCmdSetDepthTestEnable)vkGetInstanceProcAddr(m_Instance, "vkCmdSetDepthTestEnable");
        vkCmdSetDepthTestEnable(GetCurrentCommandBuffer(), false);

        const PFN_vkCmdSetDepthWriteEnable vkCmdSetDepthWriteEnable = (PFN_vkCmdSetDepthWriteEnable)vkGetInstanceProcAddr(m_Instance, "vkCmdSetDepthWriteEnable");
        vkCmdSetDepthWriteEnable(GetCurrentCommandBuffer(), false);

        const PFN_vkCmdSetStencilTestEnable vkCmdSetStencilTestEnable = (PFN_vkCmdSetStencilTestEnable)vkGetInstanceProcAddr(m_Instance, "vkCmdSetStencilTestEnable");
        vkCmdSetStencilTestEnable(GetCurrentCommandBuffer(), false);

        const PFN_vkCmdSetDepthBiasEnable vkCmdSetDepthBiasEnable = (PFN_vkCmdSetDepthBiasEnable)vkGetInstanceProcAddr(m_Instance, "vkCmdSetDepthBiasEnable");
        vkCmdSetDepthBiasEnable(GetCurrentCommandBuffer(), false);

        const f32 Width = g_Application->GetWindow()->GetWidth<f32>();
        const f32 Height = g_Application->GetWindow()->GetHeight<f32>();

        const VkViewport Viewport{0.0f, Height, Width, -Height, 0.0f, 1.0f };
        vkCmdSetViewportWithCount(GetCurrentCommandBuffer(), 1, &Viewport);

        VkRect2D Scissor;
        Scissor.extent = { (u32)Width, (u32)Height };
        Scissor.offset = { 0, 0 };
        vkCmdSetScissorWithCount(GetCurrentCommandBuffer(), 1, &Scissor);

        vkCmdSetPrimitiveTopology(GetCurrentCommandBuffer(), VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        vkCmdSetPrimitiveRestartEnable(GetCurrentCommandBuffer(), false);

        const PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT = (PFN_vkCmdSetPolygonModeEXT)vkGetInstanceProcAddr(m_Instance, "vkCmdSetPolygonModeEXT");
        vkCmdSetPolygonModeEXT(GetCurrentCommandBuffer(), VK_POLYGON_MODE_FILL);

        const PFN_vkCmdSetRasterizationSamplesEXT vkCmdSetRasterizationSamplesEXT = (PFN_vkCmdSetRasterizationSamplesEXT)vkGetInstanceProcAddr(m_Instance, "vkCmdSetRasterizationSamplesEXT");
        vkCmdSetRasterizationSamplesEXT(GetCurrentCommandBuffer(), VK_SAMPLE_COUNT_1_BIT);

        VkSampleMask SampleMask = 0xFFFFFFFF;
        const PFN_vkCmdSetSampleMaskEXT vkCmdSetSampleMaskEXT = (PFN_vkCmdSetSampleMaskEXT)vkGetInstanceProcAddr(m_Instance, "vkCmdSetSampleMaskEXT");
        vkCmdSetSampleMaskEXT(GetCurrentCommandBuffer(), VK_SAMPLE_COUNT_1_BIT, &SampleMask);

        const PFN_vkCmdSetAlphaToCoverageEnableEXT vkCmdSetAlphaToCoverageEnableEXT = (PFN_vkCmdSetAlphaToCoverageEnableEXT)vkGetInstanceProcAddr(m_Instance, "vkCmdSetAlphaToCoverageEnableEXT");
        vkCmdSetAlphaToCoverageEnableEXT(GetCurrentCommandBuffer(), false);


        const VkColorComponentFlags ColorFlags = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT| VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        const PFN_vkCmdSetColorWriteMaskEXT vkCmdSetColorWriteMaskEXT = (PFN_vkCmdSetColorWriteMaskEXT)vkGetInstanceProcAddr(m_Instance, "vkCmdSetColorWriteMaskEXT");
        vkCmdSetColorWriteMaskEXT(GetCurrentCommandBuffer(), 0, 1, &ColorFlags);

        SetBlending(false);

        IndexBuffer->Bind();
        VertexBuffer->Bind();
        Shader->Bind();

        vkCmdDrawIndexed(Cmd, (u32)IndexBuffer->Count(), 1, 0, 0, 0);
    }


    void VulkanRenderer::SetBlending(const bool Enabled)
    {
        const VkCommandBuffer Cmd = GetCurrentCommandBuffer();
        const VkBool32 Enables[] = { Enabled };
        m_FunctionPointers.vkCmdSetColorBlendEnableEXT(Cmd, 0, 1, Enables);
    }

    void VulkanRenderer::SetCullMode(const CullMode Mode)
    {
        const VkCommandBuffer Cmd = GetCurrentCommandBuffer();
        vkCmdSetCullMode(Cmd, ConvertCullMode(Mode));
    }

    void VulkanRenderer::SetDepthCompareOperation(const CompareOperation DepthFunction)
    {
        const VkCommandBuffer Cmd = GetCurrentCommandBuffer();
        vkCmdSetDepthCompareOp(Cmd, ConvertCompareOperation(DepthFunction));
    }

    void VulkanRenderer::SetBlendFunction(const BlendFactor ColorSource, const BlendFactor ColorDest, const BlendOperation ColorOperation, const BlendFactor AlphaSource, const BlendFactor
                                          AlphaDest, const BlendOperation AlphaOperation)
    {
        const VkCommandBuffer Cmd = GetCurrentCommandBuffer();
        const VkColorBlendEquationEXT ColorBlendEquation {
            ConvertBlendFactor(ColorSource), ConvertBlendFactor(ColorDest), ConvertBlendOperation(ColorOperation),
            ConvertBlendFactor(AlphaSource), ConvertBlendFactor(AlphaDest), ConvertBlendOperation(AlphaOperation)
        };
        m_FunctionPointers.vkCmdSetColorBlendEquationEXT(Cmd, 0, 1, &ColorBlendEquation);
    }

    void VulkanRenderer::SetBlendFunction(const BlendFactor Source, const BlendFactor Destination, const BlendOperation Operation)
    {
        SetBlendFunction(Source, Destination, Operation, Source, Destination, Operation);
    }

    void VulkanRenderer::BindPipeline(const Pipeline* Pipeline)
    {
        const VulkanPipeline* VkPipeline = static_cast<const VulkanPipeline*>(Pipeline);
        vkCmdBindPipeline(GetCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, VkPipeline->GetHandle());
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

    VkSwapchainKHR VulkanRenderer::GetSwapchain() const
    {
        return m_Swapchain;
    }

    u32 VulkanRenderer::GetGraphicsQueueFamily() const
    {
        return m_GraphicsQueueIndex;
    }

    VmaAllocator VulkanRenderer::GetAllocator() const
    {
        return m_Allocator;
    }

    BufferView<VkFrameData> VulkanRenderer::GetFrameData() const
    {
        return { m_Frames, m_ImageCount };
    }

    VkFrameData VulkanRenderer::GetCurrentFrameData() const
    {
        return m_Frames[m_CurrentFrameIndex];
    }

    VkCommandBuffer VulkanRenderer::GetCurrentCommandBuffer() const
    {
        return m_Frames[m_CurrentFrameIndex].CommandBuffer;
    }

    VkImage VulkanRenderer::GetCurrentImage() const
    {
        return m_Frames[m_CurrentFrameIndex].Image;
    }

    VkImageView VulkanRenderer::GetCurrentImageView() const
    {
        return m_Frames[m_CurrentFrameIndex].ImageView;
    }

    u32 VulkanRenderer::GetImageCount() const
    {
        return m_ImageCount;
    }

    VkDescriptorPool VulkanRenderer::GetDescriptorPool() const
    {
        return m_DescriptorPool;
    }

    VkCommandPool VulkanRenderer::GetCommandPool() const
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

    VkFormat VulkanRenderer::ConvertFormat(const Format Format)
    {
         switch (Format)
        {
        case Format::NONE:                  return VK_FORMAT_UNDEFINED;
        case Format::R8_UNORM:              return VK_FORMAT_R8_UNORM;
        case Format::R8_SNORM:              return VK_FORMAT_R8_SNORM;
        case Format::R16_USHORT:            return VK_FORMAT_R16_UINT;
        case Format::R16_SHORT:             return VK_FORMAT_R16_SINT;
        case Format::R32_FLOAT:             return VK_FORMAT_R32_SFLOAT;
        case Format::R32_UINT:              return VK_FORMAT_R32_UINT;
        case Format::R32_SINT:              return VK_FORMAT_R32_SINT;
        case Format::R8G8_UNORM:            return VK_FORMAT_R8G8_UNORM;
        case Format::R8G8_SNORM:            return VK_FORMAT_R8G8_SNORM;
        case Format::R16G16_USHORT:         return VK_FORMAT_R16G16_UINT;
        case Format::R16G16_SHORT:          return VK_FORMAT_R16G16_SINT;
        case Format::R32G32_UINT:           return VK_FORMAT_R32G32_UINT;
        case Format::R32G32_SINT:           return VK_FORMAT_R32G32_SINT;
        case Format::R32G32_FLOAT:          return VK_FORMAT_R32G32_SFLOAT;
        case Format::R8G8B8_UNORM:          return VK_FORMAT_R8G8B8_UNORM;
        case Format::R8G8B8_SNORM:          return VK_FORMAT_R8G8B8_SNORM;
        case Format::R16G16B16_USHORT:      return VK_FORMAT_R16G16B16_UINT;
        case Format::R16G16B16_SHORT:       return VK_FORMAT_R16G16B16_SINT;
        case Format::R32G32B32_UINT:        return VK_FORMAT_R32G32B32_UINT;
        case Format::R32G32B32_SINT:        return VK_FORMAT_R32G32B32_SINT;
        case Format::R32G32B32_FLOAT:       return VK_FORMAT_R32G32B32_SFLOAT;
        case Format::R8G8B8A8_UNORM:        return VK_FORMAT_R8G8B8A8_UNORM;
        case Format::R8G8B8A8_SNORM:        return VK_FORMAT_R8G8B8A8_SNORM;
        case Format::R16G16B16A16_USHORT:   return VK_FORMAT_R16G16B16A16_UINT;
        case Format::R16G16B16A16_SHORT:    return VK_FORMAT_R16G16B16A16_SINT;
        case Format::R32G32B32A32_UINT:     return VK_FORMAT_R32G32B32A32_UINT;
        case Format::R32G32B32A32_SINT:     return VK_FORMAT_R32G32B32A32_SINT;
        case Format::R32G32B32A32_FLOAT:    return VK_FORMAT_R32G32B32A32_SFLOAT;
        default: return VK_FORMAT_UNDEFINED;
        }
    }

    VkFilter VulkanRenderer::ConvertFilter(const Filter Filter)
    {
        switch (Filter) {
        case Filter::Nearest: return VK_FILTER_NEAREST;
        case Filter::Linear: return VK_FILTER_LINEAR;
        }
        throw;
    }

    VkSamplerAddressMode VulkanRenderer::ConvertSamplerAddressMode(const SamplerAddressMode Wrap)
    {
        switch (Wrap)
        {
        case SamplerAddressMode::Repeat: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case SamplerAddressMode::MirroredRepeat: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case SamplerAddressMode::ClampToEdge: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case SamplerAddressMode::ClampToBorder: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        case SamplerAddressMode::MirrorClampToEdge: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        }
        throw;
    }

    VkCullModeFlags VulkanRenderer::ConvertCullMode(const CullMode Mode)
    {
        switch (Mode)
        {
        case CullMode::FrontFace:           return VK_CULL_MODE_FRONT_BIT;
        case CullMode::BackFace:            return VK_CULL_MODE_BACK_BIT;
        case CullMode::FrontAndBackFaces:   return VK_CULL_MODE_FRONT_BIT | VK_CULL_MODE_BACK_BIT;
        case CullMode::None:                return VK_CULL_MODE_NONE;
        }
        return VK_CULL_MODE_NONE;
    }

    VkBlendFactor VulkanRenderer::ConvertBlendFactor(const BlendFactor Mode)
    {
        switch (Mode) {
        case BlendFactor::Zero: return VK_BLEND_FACTOR_ZERO;
        case BlendFactor::One: return VK_BLEND_FACTOR_ONE;
        case BlendFactor::SourceColor: return VK_BLEND_FACTOR_SRC_COLOR;
        case BlendFactor::OneMinusSourceColor: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DestColor: return VK_BLEND_FACTOR_DST_COLOR;
        case BlendFactor::OneMinusDestColor: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case BlendFactor::SourceAlpha: return VK_BLEND_FACTOR_SRC_ALPHA;
        case BlendFactor::OneMinusSourceAlpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DestAlpha: return VK_BLEND_FACTOR_DST_ALPHA;
        case BlendFactor::OneMinusDestAlpha: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case BlendFactor::ConstantColor: return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case BlendFactor::OnMinusConstantColor: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case BlendFactor::ConstantAlpha: return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case BlendFactor::OneMinusConstantAlpha: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        case BlendFactor::SourceAlphaSaturated: return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        case BlendFactor::Source1Color: return VK_BLEND_FACTOR_SRC1_COLOR;
        case BlendFactor::OneMinusSource1Color: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        case BlendFactor::Source1Alpha: return VK_BLEND_FACTOR_SRC1_ALPHA;
        case BlendFactor::OneMinusSource1Alpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        }
        throw;
    }

    VkCompareOp VulkanRenderer::ConvertCompareOperation(const CompareOperation Func)
    {
        switch (Func)
        {
        case CompareOperation::Always:         return VK_COMPARE_OP_ALWAYS;
        case CompareOperation::Never:          return VK_COMPARE_OP_NEVER;
        case CompareOperation::Less:           return VK_COMPARE_OP_LESS;
        case CompareOperation::LessOrEqual:    return VK_COMPARE_OP_LESS_OR_EQUAL;
        case CompareOperation::Equal:          return VK_COMPARE_OP_EQUAL;
        case CompareOperation::NotEqual:       return VK_COMPARE_OP_NOT_EQUAL;
        case CompareOperation::Greater:        return VK_COMPARE_OP_GREATER;
        case CompareOperation::GreaterOrEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
        }
        throw;
    }

    VkBlendOp VulkanRenderer::ConvertBlendOperation(const BlendOperation Operation)
    {
        switch (Operation)
        {
        case BlendOperation::Add: return VK_BLEND_OP_ADD;
        case BlendOperation::Min: return VK_BLEND_OP_MIN;
        case BlendOperation::Max: return VK_BLEND_OP_MAX;
        case BlendOperation::Subtract: return VK_BLEND_OP_SUBTRACT;
        case BlendOperation::ReverseSubtract: return VK_BLEND_OP_REVERSE_SUBTRACT;
        }
        throw;
    }

    VkPrimitiveTopology VulkanRenderer::ConvertTopology(PrimitiveTopology Topology)
    {
        switch (Topology)
        {
        case PrimitiveTopology::PointList: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case PrimitiveTopology::LineList: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case PrimitiveTopology::LineStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case PrimitiveTopology::TriangleList: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case PrimitiveTopology::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case PrimitiveTopology::TriangleFan: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        default: throw;
        }
    }

    VkFrontFace VulkanRenderer::ConvertFrontFace(FrontFace FrontFace)
    {
        switch (FrontFace)
        {
        case FrontFace::Clockwise: return VK_FRONT_FACE_CLOCKWISE;
        case FrontFace::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        default: throw;
        }
    }

    VkPolygonMode VulkanRenderer::ConvertPolygonMode(PolygonMode PolygonMode)
    {
        switch (PolygonMode)
        {
        case PolygonMode::Fill:; return VK_POLYGON_MODE_FILL;
        case PolygonMode::Line: return VK_POLYGON_MODE_LINE;
        case PolygonMode::Point: return VK_POLYGON_MODE_POINT;
        default: throw;
        }
    }

    VkShaderStageFlagBits VulkanRenderer::ConvertShaderStage(const ShaderStage Stage)
    {
        switch (Stage)
        {
        case ShaderStage::None: return (VkShaderStageFlagBits)0;
        case ShaderStage::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderStage::Geometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
        case ShaderStage::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
        default: throw;
        }
    }
}
