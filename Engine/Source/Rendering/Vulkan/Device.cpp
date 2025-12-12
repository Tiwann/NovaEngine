#include "Device.h"
#include "Runtime/Window.h"
#include "Runtime/DesktopWindow.h"
#include "Conversions.h"
#include "Rendering/Surface.h"
#include "Rendering/Swapchain.h"
#include "VulkanExtensions.h"
#include "Containers/StringFormat.h"
#include "Runtime/Version.h"
#include "Buffer.h"
#include "Sampler.h"
#include "Shader.h"
#include "GraphicsPipeline.h"
#include "ComputePipeline.h"
#include "Material.h"
#include "RenderTarget.h"
#include "Utils/VulkanUtils.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#define VMA_STATS_STRING_ENABLED 1
#include <vma/vk_mem_alloc.h>
#include <print>



#ifndef VK_LAYER_KHRONOS_VALIDATION_NAME
#define VK_LAYER_KHRONOS_VALIDATION_NAME "VK_LAYER_KHRONOS_validation"
#endif


namespace Nova::Vulkan
{
    bool Device::Initialize(const DeviceCreateInfo& createInfo)
    {
        if (!createInfo.window)
            return false;

        VkApplicationInfo applicationInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        applicationInfo.apiVersion = VK_API_VERSION_1_4;
        applicationInfo.pEngineName = "Nova Engine";
        applicationInfo.engineVersion = NOVA_VERSION;
        applicationInfo.pApplicationName = *createInfo.appName;
        applicationInfo.applicationVersion = 0;

        Array<const char*> layers;
        layers.Add(VK_LAYER_KHRONOS_VALIDATION_NAME);

        Array<const char*> extensions;
        extensions.Add(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);

        extensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#if defined(NOVA_DEBUG) || defined(NOVA_DEV)

#endif

        if (dynamic_cast<DesktopWindow*>(createInfo.window))
        {
            uint32_t glfwExtensionsCount;
            const char** lfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
            extensions.AddRange(lfwExtensions, glfwExtensionsCount);
        }

        VkInstanceCreateInfo instanceCreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.ppEnabledLayerNames = layers.Data();
        instanceCreateInfo.enabledLayerCount = layers.Count();
        instanceCreateInfo.ppEnabledExtensionNames = extensions.Data();
        instanceCreateInfo.enabledExtensionCount = extensions.Count();

        if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) != VK_SUCCESS)
        {
            std::println(std::cerr, "Failed to create instance!");
            return false;
        }

        if (!LoadVulkanFunctions(m_Instance))
            return false;

#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        static const decltype(VkDebugUtilsMessengerCreateInfoEXT::pfnUserCallback) messageCallback =
            [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            const VkDebugUtilsMessageTypeFlagsEXT messageTypes,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData) -> VkBool32
        {
            if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            {
                std::println(std::cerr, "[VULKAN ERROR] {}", pCallbackData->pMessage);
                return true;
            }

            if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            {
                std::println(std::cerr, "[VULKAN WARNING] {}", pCallbackData->pMessage);
                return true;
            }
            return false;
        };
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        debugMessengerCreateInfo.pfnUserCallback = messageCallback;
        if (vkCreateDebugUtilsMessenger(m_Instance, &debugMessengerCreateInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
            return false;
#endif

        VkPhysicalDevice availablePhysicalDevices[32];
        uint32_t availablePhysicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &availablePhysicalDeviceCount, nullptr);
        vkEnumeratePhysicalDevices(m_Instance, &availablePhysicalDeviceCount, availablePhysicalDevices);

        if (availablePhysicalDeviceCount == 1)
        {
            m_PhysicalDevice = availablePhysicalDevices[0];
        } else
        {
            for (size_t physicalDeviceIndex = 0; physicalDeviceIndex < availablePhysicalDeviceCount; ++physicalDeviceIndex)
            {
                const VkPhysicalDevice physicalDevice = availablePhysicalDevices[physicalDeviceIndex];
                VkPhysicalDeviceProperties properties;
                vkGetPhysicalDeviceProperties(physicalDevice, &properties);

                VkPhysicalDeviceFeatures features;
                vkGetPhysicalDeviceFeatures(physicalDevice, &features);
                if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && features.samplerAnisotropy == (VkBool32)true)
                {
                    m_PhysicalDevice = physicalDevice;
                    break;
                }
            }

            if (m_PhysicalDevice == nullptr)
                return false;
        }

        VkPhysicalDeviceProperties2 physicalDeviceProperties = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
        vkGetPhysicalDeviceProperties2(m_PhysicalDevice, &physicalDeviceProperties);
        m_DeviceVendor = physicalDeviceProperties.properties.deviceName;

        const SurfaceCreateInfo surfaceCreateInfo = SurfaceCreateInfo()
        .withDevice(this)
        .withWindow(createInfo.window);
        m_Surface = CreateSurface(surfaceCreateInfo);
        if (!m_Surface)
        {
            std::println(std::cerr, "[VULKAN] Failed to initialize surface!");
            return false;
        }


        uint32_t queueFamilyPropertiesCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties2(m_PhysicalDevice, &queueFamilyPropertiesCount, nullptr);
        Array<VkQueueFamilyProperties2> queueFamilyProperties(queueFamilyPropertiesCount);
        for (VkQueueFamilyProperties2& properties : queueFamilyProperties)
            properties.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
        vkGetPhysicalDeviceQueueFamilyProperties2(m_PhysicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.Data());

        for (uint32_t i = 0; i < queueFamilyProperties.Count(); ++i)
        {
            if (queueFamilyProperties[i].queueFamilyProperties.queueFlags & (VK_QUEUE_GRAPHICS_BIT))
            {
                m_GraphicsQueue.SetIndex(i);
            }

            if (queueFamilyProperties[i].queueFamilyProperties.queueFlags & (VK_QUEUE_COMPUTE_BIT))
            {
                m_ComputeQueue.SetIndex(i);
            }

            if (queueFamilyProperties[i].queueFamilyProperties.queueFlags & (VK_QUEUE_TRANSFER_BIT))
            {
                m_TransferQueue.SetIndex(i);
            }
        }

        for (uint32_t i = 0; i < queueFamilyProperties.Count(); ++i)
        {
            VkBool32 supportsSurface = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, m_Surface->GetHandle(), &supportsSurface);
            if (supportsSurface)
            {
                m_PresentQueue.SetIndex(i);
                break;
            }
        }

        Array<VkDeviceQueueCreateInfo> queueCreateInfos;
        const bool sameQueueIndices = m_GraphicsQueue.SameIndex(m_PresentQueue);
        static constexpr float queuePriorities[] = { 1.0f };
        if (sameQueueIndices)
        {
            VkDeviceQueueCreateInfo queueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = queuePriorities;
            queueCreateInfo.queueFamilyIndex = m_GraphicsQueue.GetIndex();
            queueCreateInfos.Add(queueCreateInfo);
        }
        else
        {
            VkDeviceQueueCreateInfo graphicsQueueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
            graphicsQueueCreateInfo.queueCount = 1;
            graphicsQueueCreateInfo.pQueuePriorities = queuePriorities;
            graphicsQueueCreateInfo.queueFamilyIndex = m_GraphicsQueue.GetIndex();
            queueCreateInfos.Add(graphicsQueueCreateInfo);

            VkDeviceQueueCreateInfo presentQueueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
            presentQueueCreateInfo.queueCount = 1;
            presentQueueCreateInfo.pQueuePriorities = queuePriorities;
            presentQueueCreateInfo.queueFamilyIndex = m_PresentQueue.GetIndex();
            queueCreateInfos.Add(presentQueueCreateInfo);
        }

        VkDeviceQueueCreateInfo computeQueueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        computeQueueCreateInfo.queueCount = 1;
        computeQueueCreateInfo.pQueuePriorities = queuePriorities;
        computeQueueCreateInfo.queueFamilyIndex = m_ComputeQueue.GetIndex();
        queueCreateInfos.Add(computeQueueCreateInfo);

        VkDeviceQueueCreateInfo transferQueueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        transferQueueCreateInfo.queueCount = 1;
        transferQueueCreateInfo.pQueuePriorities = queuePriorities;
        transferQueueCreateInfo.queueFamilyIndex = m_TransferQueue.GetIndex();
        queueCreateInfos.Add(transferQueueCreateInfo);


        Array<const char*> deviceExtensions;
        deviceExtensions.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        deviceExtensions.Add(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);


        VkPhysicalDeviceShaderDrawParametersFeatures shaderDrawParametersFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES };
        shaderDrawParametersFeatures.shaderDrawParameters = true;

        VkPhysicalDeviceSynchronization2Features synchronization2Features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES };
        synchronization2Features.synchronization2 = true;
        synchronization2Features.pNext = &shaderDrawParametersFeatures;

        VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES };
        indexingFeatures.pNext = &synchronization2Features;
        indexingFeatures.runtimeDescriptorArray = true;
        indexingFeatures.descriptorBindingVariableDescriptorCount = true;
        indexingFeatures.descriptorBindingPartiallyBound = true;
        indexingFeatures.shaderSampledImageArrayNonUniformIndexing = true;

        VkPhysicalDeviceIndexTypeUint8Features uint8Features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES };
        uint8Features.indexTypeUint8 = true;
        uint8Features.pNext = &indexingFeatures;

        VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES };
        dynamicRenderingFeatures.dynamicRendering = true;
        dynamicRenderingFeatures.pNext = &uint8Features;

        VkPhysicalDeviceFeatures features = {};
        features.samplerAnisotropy = true;
        features.fillModeNonSolid = true;
        features.wideLines = true;

        VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        deviceCreateInfo.pNext = &dynamicRenderingFeatures;
        deviceCreateInfo.pEnabledFeatures = &features;
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.Data();
        deviceCreateInfo.enabledExtensionCount = deviceExtensions.Count();
        deviceCreateInfo.enabledLayerCount = 0;
        deviceCreateInfo.ppEnabledLayerNames = nullptr;
        deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.Count();
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.Data();

        if (vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Handle) != VK_SUCCESS)
        {
            std::println(std::cerr, "Failed to create logical device!");
            return false;
        }


        VkDeviceQueueInfo2 queueInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2 };
        queueInfo.queueIndex = 0;
        queueInfo.queueFamilyIndex = m_GraphicsQueue.GetIndex();
        vkGetDeviceQueue2(m_Handle, &queueInfo, m_GraphicsQueue.GetHandlePtr());
        if (!m_GraphicsQueue.GetHandle())
        {
            std::println(std::cerr, "Failed to retrieve graphics queue!");
            return false;
        }

        queueInfo.queueIndex = 0;
        queueInfo.queueFamilyIndex = m_PresentQueue.GetIndex();
        vkGetDeviceQueue2(m_Handle, &queueInfo, m_PresentQueue.GetHandlePtr());
        if (!m_PresentQueue.GetHandle())
        {
            std::println(std::cerr, "Failed to retrieve present queue!");
            return false;
        }

        queueInfo.queueIndex = 0;
        queueInfo.queueFamilyIndex = m_ComputeQueue.GetIndex();
        vkGetDeviceQueue2(m_Handle, &queueInfo, m_ComputeQueue.GetHandlePtr());
        if (!m_ComputeQueue.GetHandle())
        {
            std::println(std::cerr, "Failed to retrieve compute queue!");
            return false;
        }

        queueInfo.queueIndex = 0;
        queueInfo.queueFamilyIndex = m_TransferQueue.GetIndex();
        vkGetDeviceQueue2(m_Handle, &queueInfo, m_TransferQueue.GetHandlePtr());
        if (!m_TransferQueue.GetHandle())
        {
            std::println(std::cerr, "Failed to retrieve transfer queue!");
            return false;
        }

        VmaAllocatorCreateInfo allocatorCreateInfo = { 0 };
        allocatorCreateInfo.device = m_Handle;
        allocatorCreateInfo.instance = m_Instance;
        allocatorCreateInfo.physicalDevice = m_PhysicalDevice;
        allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_4;
        if (vmaCreateAllocator(&allocatorCreateInfo, &m_Allocator) != VK_SUCCESS)
        {
            std::println(std::cerr, "Failed to create allocator!");
            return false;
        }

        CommandPoolCreateInfo commandPoolCreateInfo;
        commandPoolCreateInfo.device = this;
        commandPoolCreateInfo.flags = CommandPoolCreateFlagBits::ResetCommandBuffer;
        commandPoolCreateInfo.queue = &m_GraphicsQueue;
        if (!m_CommandPool.Initialize(commandPoolCreateInfo))
        {
            std::println(std::cerr, "Failed to create graphics command pool!");
            return false;
        }

        commandPoolCreateInfo.queue = &m_TransferQueue;
        if (!m_TransferCommandPool.Initialize(commandPoolCreateInfo))
        {
            std::println(std::cerr, "Failed to create transfer command pool!");
            return false;
        }

        const auto GetPresentMode = [&](const bool vSync) -> PresentMode
        {
            VkPresentModeKHR result;

            if (vSync)
            {
                result = VK_PRESENT_MODE_FIFO_KHR;

                uint32_t presentModeCount;
                vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface->GetHandle(), &presentModeCount, nullptr);
                Array<VkPresentModeKHR> presentModes(presentModeCount);
                vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface->GetHandle(), &presentModeCount, presentModes.Data());

                if (presentModes.Contains(VK_PRESENT_MODE_MAILBOX_KHR))
                    result = VK_PRESENT_MODE_MAILBOX_KHR;
            } else
            {
                result = VK_PRESENT_MODE_IMMEDIATE_KHR;
            }

            switch (result)
            {
            case VK_PRESENT_MODE_IMMEDIATE_KHR: return PresentMode::Immediate;
            case VK_PRESENT_MODE_MAILBOX_KHR: return PresentMode::Mailbox;
            case VK_PRESENT_MODE_FIFO_KHR: return PresentMode::Fifo;
            case VK_PRESENT_MODE_FIFO_RELAXED_KHR: return PresentMode::FifoRelaxed;
            default: return PresentMode::Unknown;
            };
        };

        SwapchainCreateInfo swapchainCreateInfo;
        swapchainCreateInfo.device = this;
        swapchainCreateInfo.surface = m_Surface;
        swapchainCreateInfo.recycle = false;
        swapchainCreateInfo.buffering = createInfo.buffering;
        swapchainCreateInfo.format = Format::R8G8B8A8_SRGB;
        swapchainCreateInfo.width = m_Surface->GetWidth();
        swapchainCreateInfo.height = m_Surface->GetHeight();
        swapchainCreateInfo.presentMode = GetPresentMode(createInfo.vSync);

        if (!m_Swapchain.Initialize(swapchainCreateInfo))
        {
            std::println(std::cerr, "Failed to create swapchain!");
            return false;
        }

        m_Swapchain.SetName("Swapchain");

        for (size_t imageIndex = 0; imageIndex < m_Swapchain.GetImageCount(); ++imageIndex)
        {
            SemaphoreCreateInfo semaphoreCreateInfo(this);
            m_Frames[imageIndex].submitSemaphore.Initialize(semaphoreCreateInfo);
            m_Frames[imageIndex].presentSemaphore.Initialize(semaphoreCreateInfo);

            FenceCreateInfo fenceCreateInfo;
            fenceCreateInfo.device = this;
            fenceCreateInfo.flags = FenceCreateFlagBits::None;
            m_Frames[imageIndex].fence.Initialize(fenceCreateInfo);

            CommandBufferAllocateInfo allocateInfo;
            allocateInfo.device = this;
            allocateInfo.level = CommandBufferLevel::Primary;
            allocateInfo.commandPool = &m_CommandPool;

            CommandBuffer commandBuffer = m_CommandPool.AllocateCommandBuffer(CommandBufferLevel::Primary);
            commandBuffer.SetName(StringFormat("Main Commander Buffer ({})", imageIndex));
            m_Frames[imageIndex].commandBuffer = commandBuffer;
        }

        DescriptorPoolCreateInfo descriptorPoolCreateInfo = DescriptorPoolCreateInfo()
        .SetDevice(this)
        .SetBindingTypeSize(BindingType::Sampler, 32)
        .SetBindingTypeSize(BindingType::SampledTexture, 512)
        .SetBindingTypeSize(BindingType::StorageTexture, 32)
        .SetBindingTypeSize(BindingType::CombinedTextureSampler, 512)
        .SetBindingTypeSize(BindingType::UniformBuffer, 32)
        .SetBindingTypeSize(BindingType::StorageBuffer, 32)
        .SetMaxSets(4096);
        m_DescriptorPool.Initialize(descriptorPoolCreateInfo);
        return true;
    }

    void Device::Destroy()
    {
        for (size_t imageIndex = 0; imageIndex < m_Swapchain.GetImageCount(); ++imageIndex)
        {
            m_Frames[imageIndex].submitSemaphore.Destroy();
            m_Frames[imageIndex].presentSemaphore.Destroy();
            m_Frames[imageIndex].fence.Destroy();
            m_Frames[imageIndex].commandBuffer.Free();
        }

        m_CommandPool.Destroy();
        m_TransferCommandPool.Destroy();
        m_Swapchain.Destroy();
        vmaDestroyAllocator(m_Allocator);
        m_Surface->Destroy();
        vkDestroyDevice(m_Handle, nullptr);
        vkDestroyInstance(m_Instance, nullptr);
    }


    bool Device::BeginFrame()
    {
        if (!m_Surface->IsAvailable())
            return false;

        if (!m_Swapchain.IsValid())
        {
            WaitIdle();
            m_Swapchain.Recreate();
            m_CurrentFrameIndex = 0;
            return false;
        }

        Fence& fence = m_Frames[m_LastFrameIndex].fence;
        fence.Wait(1000000000);
        fence.Reset();

        const Semaphore& presentSemaphore = m_Frames[m_LastFrameIndex].presentSemaphore;
        if (!m_Swapchain.AcquireNextImage(&presentSemaphore, nullptr, m_CurrentFrameIndex))
        {
            m_Swapchain.Invalidate();
            return false;
        }

        CommandBuffer& commandBuffer = m_Frames[m_CurrentFrameIndex].commandBuffer;
        if (!commandBuffer.Begin({ CommandBufferUsageFlagBits::OneTimeSubmit }))
            return false;


        VkImageMemoryBarrier2 barrier { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier.srcAccessMask = VK_ACCESS_2_NONE;
        barrier.dstAccessMask = VK_ACCESS_2_NONE;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = m_Swapchain.GetImage(m_CurrentFrameIndex);
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcStageMask = 0;
        barrier.dstStageMask = 0;

        VkDependencyInfo dependency = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
        dependency.dependencyFlags = 0;
        dependency.imageMemoryBarrierCount = 1;
        dependency.pImageMemoryBarriers = &barrier;
        vkCmdPipelineBarrier2(commandBuffer.GetHandle(), &dependency);

        return true;
    }

    void Device::EndFrame()
    {
        CommandBuffer& commandBuffer = GetCurrentCommandBuffer();

        VkImageMemoryBarrier2 barrier { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcAccessMask = VK_ACCESS_2_NONE;
        barrier.dstAccessMask = VK_ACCESS_2_NONE;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = m_Swapchain.GetImage(m_CurrentFrameIndex);
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcStageMask = 0;
        barrier.dstStageMask = 0;

        VkDependencyInfo inDependency = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
        inDependency.dependencyFlags = 0;
        inDependency.imageMemoryBarrierCount = 1;
        inDependency.pImageMemoryBarriers = &barrier;
        vkCmdPipelineBarrier2(commandBuffer.GetHandle(), &inDependency);

        Fence& fence = GetCurrentFence();
        Semaphore& submitSemaphore = GetCurrentSubmitSemaphore();
        Semaphore& presentSemaphore = m_Frames[m_LastFrameIndex].presentSemaphore;
        commandBuffer.End();

        m_GraphicsQueue.Submit(&commandBuffer, &presentSemaphore, &submitSemaphore, &fence, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    }

    void Device::Present()
    {
        const Semaphore& submitSemaphore = GetCurrentSubmitSemaphore();
        (void)m_GraphicsQueue.Present(m_Swapchain, &submitSemaphore, m_CurrentFrameIndex);
        m_LastFrameIndex = m_CurrentFrameIndex;
    }

    void Device::WaitIdle() const
    {
        vkDeviceWaitIdle(m_Handle);
    }

    void Device::SetName(StringView name)
    {
        SetVkObjectDebugName(this, VK_OBJECT_TYPE_DEVICE, m_Handle, name);
    }

    DeviceType Device::GetDeviceType()
    {
        return DeviceType::Vulkan;
    }

    Ref<Nova::Surface> Device::CreateSurface(const SurfaceCreateInfo& createInfo)
    {
        Surface* surface = new Surface();
        SurfaceCreateInfo surfaceCreateInfo(createInfo);
        if (!surface->Initialize(surfaceCreateInfo.withDevice(this)))
        {
            delete surface;
            return nullptr;
        }
        return Ref(surface);
    }

    uint32_t Device::GetImageCount() const
    {
        return m_Swapchain.GetImageCount();

    }

    Ref<Nova::RenderTarget> Device::CreateRenderTarget(const RenderTargetCreateInfo& createInfo)
    {
        RenderTarget* renderTarget = new RenderTarget;
        if (!renderTarget->Initialize(createInfo))
        {
            delete renderTarget;
            return nullptr;
        }
        return Ref(renderTarget);
    }

    Ref<Nova::Texture> Device::CreateTexture(const TextureCreateInfo& createInfo)
    {
        Texture* texture = new Texture();
        TextureCreateInfo textureCreateInfo(createInfo);
        if (!texture->Initialize(textureCreateInfo.WithDevice(this)))
        {
            delete texture;
            return nullptr;
        }
        return Ref(texture);
    }

    Ref<Nova::Texture> Device::CreateTextureUnitialized()
    {
        return MakeRef<Texture>();
    }

    Ref<Nova::Sampler> Device::CreateSampler(const SamplerCreateInfo& createInfo)
    {
        Sampler* sampler = new Sampler();
        SamplerCreateInfo samplerCreateInfo(createInfo);
        if (!sampler->Initialize(samplerCreateInfo.WithDevice(this)))
        {
            delete sampler;
            return nullptr;
        }
        return Ref(sampler);
    }

    Ref<Nova::Buffer> Device::CreateBuffer(const BufferCreateInfo& createInfo)
    {
        Buffer* buffer = new Buffer();
        BufferCreateInfo bufferCreateInfo(createInfo);
        if (!buffer->Initialize(bufferCreateInfo.WithDevice(this)))
        {
            delete buffer;
            return nullptr;
        }
        return Ref(buffer);
    }

    Ref<Nova::GraphicsPipeline> Device::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo)
    {
        GraphicsPipeline* pipeline = new GraphicsPipeline();
        GraphicsPipelineCreateInfo pipelineCreateInfo(createInfo);
        if (!pipeline->Initialize(pipelineCreateInfo.SetDevice(this)))
        {
            delete pipeline;
            return nullptr;
        }
        return Ref<GraphicsPipeline>(pipeline);
    }

    Ref<Nova::ComputePipeline> Device::CreateComputePipeline(const ComputePipelineCreateInfo& createInfo)
    {
        ComputePipeline* pipeline = new ComputePipeline();
        ComputePipelineCreateInfo pipelineCreateInfo(createInfo);
        if (!pipeline->Initialize(pipelineCreateInfo.WithDevice(this)))
        {
            delete pipeline;
            return nullptr;
        }
        return Ref(pipeline);
    }

    Ref<Nova::Shader> Device::CreateShader(const ShaderCreateInfo& createInfo)
    {
        Shader* shader = new Shader();
        ShaderCreateInfo shaderCreateInfo(createInfo);
        if (!shader->Initialize(shaderCreateInfo.WithDevice(this)))
        {
            delete shader;
            return nullptr;
        }
        return Ref(shader);
    }

    Ref<Nova::Material> Device::CreateMaterial(const MaterialCreateInfo& createInfo)
    {
        Material* material = new Material();
        MaterialCreateInfo matCreateInfo(createInfo);
        if (!material->Initialize(matCreateInfo.WithDevice(this)))
        {
            delete material;
            return nullptr;
        }
        return Ref(material);
    }

    Ref<Nova::Fence> Device::CreateFence(const FenceCreateInfo& createInfo)
    {
        Fence* fence = new Fence();
        FenceCreateInfo fenceCreateInfo(createInfo);
        if (!fence->Initialize(fenceCreateInfo.WithDevice(this)))
        {
            delete fence;
            return nullptr;
        }
        return Ref(fence);
    }

    VkInstance Device::GetInstance() const
    {
        return m_Instance;
    }

    VkDevice Device::GetHandle() const
    {
        return m_Handle;
    }

    VmaAllocator Device::GetAllocator() const
    {
        return m_Allocator;
    }

    VkPhysicalDevice Device::GetPhysicalDevice() const
    {
        return m_PhysicalDevice;
    }

    Ref<Nova::Surface> Device::GetSurface()
    {
        return m_Surface;
    }


    CommandPool* Device::GetCommandPool()
    {
        return &m_CommandPool;
    }

    CommandPool* Device::GetTransferCommandPool()
    {
        return &m_TransferCommandPool;
    }

    Queue* Device::GetGraphicsQueue()
    {
        return &m_GraphicsQueue;
    }

    Queue* Device::GetPresentQueue()
    {
        return &m_PresentQueue;
    }

    Queue* Device::GetComputeQueue()
    {
        return &m_ComputeQueue;
    }

    Queue* Device::GetTransferQueue()
    {
        return &m_TransferQueue;
    }

    DescriptorPool* Device::GetDescriptorPool()
    {
        return &m_DescriptorPool;
    }

    Semaphore& Device::GetCurrentSubmitSemaphore()
    {
        return m_Frames[m_CurrentFrameIndex].submitSemaphore;
    }

    Semaphore& Device::GetCurrentPresentSemaphore()
    {
        return m_Frames[m_CurrentFrameIndex].presentSemaphore;
    }

    Fence& Device::GetCurrentFence()
    {
        return m_Frames[m_CurrentFrameIndex].fence;
    }

    CommandBuffer& Device::GetCurrentCommandBuffer()
    {
        return m_Frames[m_CurrentFrameIndex].commandBuffer;
    }

    uint32_t Device::GetCurrentFrameIndex() const
    {
        return m_CurrentFrameIndex;
    }

    Nova::Swapchain* Device::GetSwapchain()
    {
        return &m_Swapchain;
    }
}
