#include "Device.h"
#include "Runtime/Window.h"
#include "Runtime/DesktopWindow.h"
#include "Conversions.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "Rendering/Surface.h"
#include "Rendering/Swapchain.h"
#include "VulkanExtensions.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vma/vk_mem_alloc.h>

#include "Containers/StringFormat.h"

#ifndef VK_LAYER_KHRONOS_VALIDATION_NAME
#define VK_LAYER_KHRONOS_VALIDATION_NAME "VK_LAYER_KHRONOS_validation"
#endif

namespace Nova::Vulkan
{
    bool Device::Initialize(const Rendering::DeviceCreateInfo& createInfo)
    {
        if (!createInfo.window)
            return false;

        VkApplicationInfo applicationInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        applicationInfo.apiVersion = VK_API_VERSION_1_4;
        applicationInfo.pEngineName = "Nova Engine";
        applicationInfo.engineVersion = createInfo.versionMajor << 16 | (createInfo.versionMinor & 0xFF);
        applicationInfo.pApplicationName = *createInfo.applicationName;
        applicationInfo.applicationVersion = 0;

        Array<const char*> layers;
        layers.Add(VK_LAYER_KHRONOS_VALIDATION_NAME);

        Array<const char*> extensions;
        extensions.Add(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);

#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        extensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        if (dynamic_cast<DesktopWindow*>(createInfo.window))
        {
            uint32_t GlfwExtensionCount;
            const char** GlfwExtensions = glfwGetRequiredInstanceExtensions(&GlfwExtensionCount);
            extensions.AddRange(GlfwExtensions, GlfwExtensionCount);
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


        Rendering::SurfaceCreateInfo surfaceCreateInfo;
        surfaceCreateInfo.window = createInfo.window;
        surfaceCreateInfo.device = this;
        if (!m_Surface.Initialize(surfaceCreateInfo))
        {
            std::println(std::cerr, "Failed to initialize surface!");
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
            vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, m_Surface.GetHandle(), &supportsSurface);
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

        VkPhysicalDeviceSynchronization2Features synchronization2Features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES };
        synchronization2Features.synchronization2 = true;


        VkPhysicalDeviceDescriptorIndexingFeaturesEXT indexingFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT };
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

        Rendering::CommandPoolCreateInfo commandPoolCreateInfo;
        commandPoolCreateInfo.device = this;
        commandPoolCreateInfo.flags = Rendering::CommandPoolCreateFlagBits::ResetCommandBuffer;
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
                vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface.GetHandle(), &presentModeCount, nullptr);
                Array<VkPresentModeKHR> presentModes(presentModeCount);
                vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface.GetHandle(), &presentModeCount, presentModes.Data());

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

        Rendering::SwapchainCreateInfo swapchainCreateInfo;
        swapchainCreateInfo.device = this;
        swapchainCreateInfo.surface = &m_Surface;
        swapchainCreateInfo.recycle = false;
        swapchainCreateInfo.buffering = createInfo.buffering;
        swapchainCreateInfo.format = Format::R8G8B8A8_UNORM;
        swapchainCreateInfo.width = createInfo.window->GetWidth();
        swapchainCreateInfo.height = createInfo.window->GetHeight();
        swapchainCreateInfo.presentMode = GetPresentMode(createInfo.vSync);

        if (!m_Swapchain.Initialize(swapchainCreateInfo))
        {
            std::println(std::cerr, "Failed to create swapchain!");
            return false;
        }

        m_Swapchain.SetName("Swapchain");

        for (size_t imageIndex = 0; imageIndex < m_Swapchain.GetImageCount(); ++imageIndex)
        {
            Rendering::SemaphoreCreateInfo semaphoreCreateInfo(this);
            m_Frames[imageIndex].submitSemaphore.Initialize(semaphoreCreateInfo);
            m_Frames[imageIndex].presentSemaphore.Initialize(semaphoreCreateInfo);

            Rendering::FenceCreateInfo fenceCreateInfo;
            fenceCreateInfo.device = this;
            fenceCreateInfo.flags = Rendering::FenceCreateFlagBits::None;
            m_Frames[imageIndex].fence.Initialize(fenceCreateInfo);

            Rendering::CommandBufferAllocateInfo allocateInfo;
            allocateInfo.device = this;
            allocateInfo.level = Rendering::CommandBufferLevel::Primary;
            allocateInfo.commandPool = &m_CommandPool;

            CommandBuffer commandBuffer = m_CommandPool.AllocateCommandBuffer(Rendering::CommandBufferLevel::Primary);
            commandBuffer.SetName(StringFormat("Main Commander Buffer ({})", imageIndex));
            m_Frames[imageIndex].commandBuffer = commandBuffer;
        }

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
        m_Surface.Destroy();
        vkDestroyDevice(m_Handle, nullptr);
        vkDestroyInstance(m_Instance, nullptr);
    }


    bool Device::BeginFrame()
    {
        if (!m_Surface.IsAvailable())
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
        if (!commandBuffer.Begin({ Rendering::CommandBufferUsageFlagBits::OneTimeSubmit }))
            return false;


        VkImageMemoryBarrier2 barrier { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
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
        (void)m_GraphicsQueue.Present(m_Swapchain, submitSemaphore, m_CurrentFrameIndex);
        m_LastFrameIndex = m_CurrentFrameIndex;
    }

    void Device::WaitIdle()
    {
        vkDeviceWaitIdle(m_Handle);
    }

    void Device::SetName(StringView name)
    {
#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        VkDebugUtilsObjectNameInfoEXT info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
        info.objectHandle = (uint64_t)m_Handle;
        info.objectType = VK_OBJECT_TYPE_DEVICE;
        info.pObjectName = *name;
        vkSetDebugUtilsObjectName(m_Handle, &info);
#endif
    }

    void Device::ResolveToSwapchain(Rendering::RenderTarget& renderTarget)
    {
        const VkImage swapchainImage = m_Swapchain.GetImage(m_CurrentFrameIndex);
        const CommandBuffer& commandBuffer = GetCurrentCommandBuffer();
        const RenderTarget& rt = static_cast<RenderTarget&>(renderTarget);

        VkImageMemoryBarrier transferBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        transferBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        transferBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        transferBarrier.srcAccessMask = 0;
        transferBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        transferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        transferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        transferBarrier.image = swapchainImage;
        transferBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        transferBarrier.subresourceRange.baseMipLevel = 0;
        transferBarrier.subresourceRange.levelCount = 1;
        transferBarrier.subresourceRange.baseArrayLayer = 0;
        transferBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            commandBuffer.GetHandle(),
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &transferBarrier
        );

        VkImageResolve region;
        region.extent.width = rt.GetWidth();
        region.extent.height = rt.GetHeight();
        region.extent.depth = 1;
        region.dstOffset = VkOffset3D{ 0, 0, 0 };
        region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.dstSubresource.baseArrayLayer = 0;
        region.dstSubresource.layerCount = 1;
        region.dstSubresource.mipLevel = 0;
        region.srcOffset = VkOffset3D{ 0, 0, 0 };
        region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.srcSubresource.baseArrayLayer = 0;
        region.srcSubresource.layerCount = 1;
        region.srcSubresource.mipLevel = 0;

        const VkCommandBuffer cmdBuff = commandBuffer.GetHandle();
        const VkImage srcImage = rt.GetColorImage(m_CurrentFrameIndex);
        const VkImage dstImage = swapchainImage;
        vkCmdResolveImage(cmdBuff, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        VkImageMemoryBarrier presentBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        presentBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        presentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        presentBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        presentBarrier.dstAccessMask = VK_ACCESS_NONE;
        presentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        presentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        presentBarrier.image = swapchainImage;
        presentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        presentBarrier.subresourceRange.baseMipLevel = 0;
        presentBarrier.subresourceRange.levelCount = 1;
        presentBarrier.subresourceRange.baseArrayLayer = 0;
        presentBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            commandBuffer.GetHandle(),
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0, 0, nullptr, 0, nullptr, 1, &presentBarrier
        );
    }

    void Device::BlitToSwapchain(Rendering::RenderTarget& renderTarget, const Filter filter)
    {
        const VkImage swapchainImage = m_Swapchain.GetImage(m_CurrentFrameIndex);
        const CommandBuffer& commandBuffer = GetCurrentCommandBuffer();
        const RenderTarget& rt = static_cast<RenderTarget&>(renderTarget);

        VkImageMemoryBarrier transferBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        transferBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        transferBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        transferBarrier.srcAccessMask = 0;
        transferBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        transferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        transferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        transferBarrier.image = swapchainImage;
        transferBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        transferBarrier.subresourceRange.baseMipLevel = 0;
        transferBarrier.subresourceRange.levelCount = 1;
        transferBarrier.subresourceRange.baseArrayLayer = 0;
        transferBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            commandBuffer.GetHandle(),
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &transferBarrier
        );

        const int32_t srcWidth = rt.GetWidth();
        const int32_t srcHeight = rt.GetHeight();
        const int32_t dstWidth = m_Swapchain.GetWidth();
        const int32_t dstHeight = m_Swapchain.GetHeight();
        VkImageBlit blit;
        blit.srcOffsets[0] = VkOffset3D{ 0, 0, 0 };
        blit.srcOffsets[1] = VkOffset3D{ srcWidth, srcHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.srcSubresource.mipLevel = 0;
        blit.dstOffsets[0] = VkOffset3D{ 0, 0, 0 };
        blit.dstOffsets[1] = VkOffset3D{ dstWidth, dstHeight, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;
        blit.dstSubresource.mipLevel = 0;

        const VkCommandBuffer cmdBuff = commandBuffer.GetHandle();
        const VkImage srcImage = rt.GetColorImage(m_CurrentFrameIndex);
        const VkImage dstImage = swapchainImage;
        vkCmdBlitImage(cmdBuff, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, Convert<Filter, VkFilter>(filter));

        VkImageMemoryBarrier presentBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        presentBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        presentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        presentBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        presentBarrier.dstAccessMask = VK_ACCESS_NONE;
        presentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        presentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        presentBarrier.image = swapchainImage;
        presentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        presentBarrier.subresourceRange.baseMipLevel = 0;
        presentBarrier.subresourceRange.levelCount = 1;
        presentBarrier.subresourceRange.baseArrayLayer = 0;
        presentBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            commandBuffer.GetHandle(),
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0, 0, nullptr, 0, nullptr, 1, &presentBarrier
        );
    }

    void Device::BlitToSwapchain(Rendering::Texture& texture, const Filter filter)
    {
        const VkImage swapchainImage = m_Swapchain.GetImage(m_CurrentFrameIndex);
        const CommandBuffer& commandBuffer = GetCurrentCommandBuffer();
        const Texture& tex = (Texture&)texture;

        VkImageMemoryBarrier transferBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        transferBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        transferBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        transferBarrier.srcAccessMask = VK_ACCESS_NONE;
        transferBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        transferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        transferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        transferBarrier.image = swapchainImage;
        transferBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        transferBarrier.subresourceRange.baseMipLevel = 0;
        transferBarrier.subresourceRange.levelCount = 1;
        transferBarrier.subresourceRange.baseArrayLayer = 0;
        transferBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            commandBuffer.GetHandle(),
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &transferBarrier
        );

        const int32_t srcWidth = tex.GetWidth();
        const int32_t srcHeight = tex.GetHeight();
        const int32_t dstWidth = m_Swapchain.GetWidth();
        const int32_t dstHeight = m_Swapchain.GetHeight();
        VkImageBlit blit;
        blit.srcOffsets[0] = VkOffset3D{ 0, 0, 0 };
        blit.srcOffsets[1] = VkOffset3D{ srcWidth, srcHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.srcSubresource.mipLevel = 0;
        blit.dstOffsets[0] = VkOffset3D{ 0, 0, 0 };
        blit.dstOffsets[1] = VkOffset3D{ dstWidth, dstHeight, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;
        blit.dstSubresource.mipLevel = 0;

        const VkCommandBuffer cmdBuff = commandBuffer.GetHandle();
        const VkImage srcImage = tex.GetImage();
        const VkImage dstImage = swapchainImage;
        vkCmdBlitImage(cmdBuff, srcImage, (VkImageLayout)tex.GetImageLayout(), dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, Convert<Filter, VkFilter>(filter));

        VkImageMemoryBarrier presentBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        presentBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        presentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        presentBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        presentBarrier.dstAccessMask = VK_ACCESS_NONE;
        presentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        presentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        presentBarrier.image = swapchainImage;
        presentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        presentBarrier.subresourceRange.baseMipLevel = 0;
        presentBarrier.subresourceRange.levelCount = 1;
        presentBarrier.subresourceRange.baseArrayLayer = 0;
        presentBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            commandBuffer.GetHandle(),
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0, 0, nullptr, 0, nullptr, 1, &presentBarrier
        );
    }

    void Device::BlitToRenderTarget(Rendering::Texture& srcTexture, Rendering::RenderTarget& destRenderTarget, uint32_t x, uint32_t y)
    {
        const Texture& src = static_cast<Texture&>(srcTexture);
        const RenderTarget& dest = static_cast<RenderTarget&>(destRenderTarget);

        const CommandBuffer& commandBuffer = GetCurrentCommandBuffer();

        VkImageMemoryBarrier srcToTransferSourceBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        srcToTransferSourceBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        srcToTransferSourceBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        srcToTransferSourceBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        srcToTransferSourceBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        srcToTransferSourceBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        srcToTransferSourceBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        srcToTransferSourceBarrier.image = src.GetImage();
        srcToTransferSourceBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        srcToTransferSourceBarrier.subresourceRange.baseMipLevel = 0;
        srcToTransferSourceBarrier.subresourceRange.levelCount = src.GetMips();
        srcToTransferSourceBarrier.subresourceRange.baseArrayLayer = 0;
        srcToTransferSourceBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            commandBuffer.GetHandle(),
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &srcToTransferSourceBarrier
        );

        VkImageMemoryBarrier destToTransferDestBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        destToTransferDestBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        destToTransferDestBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        destToTransferDestBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        destToTransferDestBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        destToTransferDestBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        destToTransferDestBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        destToTransferDestBarrier.image = dest.GetColorImage(m_CurrentFrameIndex);
        destToTransferDestBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        destToTransferDestBarrier.subresourceRange.baseMipLevel = 0;
        destToTransferDestBarrier.subresourceRange.levelCount = 1;
        destToTransferDestBarrier.subresourceRange.baseArrayLayer = 0;
        destToTransferDestBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            commandBuffer.GetHandle(),
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &destToTransferDestBarrier
        );

        const int32_t srcWidth = src.GetWidth();
        const int32_t srcHeight = src.GetHeight();
        const int32_t dstWidth = dest.GetWidth();
        const int32_t dstHeight = dest.GetHeight();

        VkImageBlit blit;
        blit.srcOffsets[0] = VkOffset3D{ 0, 0, 0 };
        blit.srcOffsets[1] = VkOffset3D{ srcWidth, srcHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.srcSubresource.mipLevel = 0;
        blit.dstOffsets[0] = VkOffset3D{ (int32_t)x, (int32_t)y, 0 };
        blit.dstOffsets[1] = VkOffset3D{ srcWidth, srcHeight, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;
        blit.dstSubresource.mipLevel = 0;

        const VkCommandBuffer cmdBuff = commandBuffer.GetHandle();
        const VkImage srcImage = src.GetImage();
        const VkImage dstImage = dest.GetColorImage(m_CurrentFrameIndex);
        vkCmdBlitImage(cmdBuff, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, Convert<Filter, VkFilter>(Filter::Nearest));

        VkImageMemoryBarrier srcToShaderReadBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        srcToShaderReadBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        srcToShaderReadBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        srcToShaderReadBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        srcToShaderReadBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        srcToShaderReadBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        srcToShaderReadBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        srcToShaderReadBarrier.image = src.GetImage();
        srcToShaderReadBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        srcToShaderReadBarrier.subresourceRange.baseMipLevel = 0;
        srcToShaderReadBarrier.subresourceRange.levelCount = src.GetMips();
        srcToShaderReadBarrier.subresourceRange.baseArrayLayer = 0;
        srcToShaderReadBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            commandBuffer.GetHandle(),
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &srcToShaderReadBarrier
        );

        VkImageMemoryBarrier destToTransferSourceBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        destToTransferSourceBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        destToTransferSourceBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        destToTransferSourceBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        destToTransferSourceBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        destToTransferSourceBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        destToTransferSourceBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        destToTransferSourceBarrier.image = dest.GetColorImage(m_CurrentFrameIndex);
        destToTransferSourceBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        destToTransferSourceBarrier.subresourceRange.baseMipLevel = 0;
        destToTransferSourceBarrier.subresourceRange.levelCount = 1;
        destToTransferSourceBarrier.subresourceRange.baseArrayLayer = 0;
        destToTransferSourceBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            commandBuffer.GetHandle(),
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &destToTransferSourceBarrier
        );
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

    Surface* Device::GetSurface()
    {
        return &m_Surface;
    }

    Swapchain* Device::GetSwapchain()
    {
        return &m_Swapchain;
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
}
