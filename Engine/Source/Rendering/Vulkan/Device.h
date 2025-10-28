#pragma once
#include "Rendering/Device.h"
#include "Queue.h"
#include "Surface.h"
#include "Swapchain.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "DescriptorPool.h"
#include "Fence.h"
#include "Semaphore.h"
#include "Containers/MulticastDelegate.h"

typedef struct VkInstance_T* VkInstance;
typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
typedef struct VkDevice_T* VkDevice;
typedef struct VmaAllocator_T* VmaAllocator;
typedef struct VkDebugUtilsMessengerEXT_T* VkDebugUtilsMessengerEXT;

namespace Nova::Vulkan
{
    struct Frame
    {
        Semaphore submitSemaphore;
        Semaphore presentSemaphore;
        Fence fence;
        CommandBuffer commandBuffer;
    };

    class Device : public Rendering::Device
    {
    public:
        bool Initialize(const Rendering::DeviceCreateInfo& createInfo) override;
        void Destroy() override;

        bool BeginFrame() override;
        void EndFrame() override;
        void Present() override;
        void WaitIdle() const override;
        void SetName(StringView name) override;
        Rendering::DeviceType GetDeviceType() override;
        uint32_t GetImageCount() const override;

        Ref<Rendering::Surface> CreateSurface(const Rendering::SurfaceCreateInfo& createInfo) override;
        Ref<Rendering::Texture> CreateTexture(const Rendering::TextureCreateInfo& createInfo) override;
        Ref<Rendering::Sampler> CreateSampler(const Rendering::SamplerCreateInfo& createInfo) override;
        Ref<Rendering::Buffer> CreateBuffer(const Rendering::BufferCreateInfo& createInfo) override;
        Ref<Rendering::GraphicsPipeline> CreateGraphicsPipeline(const Rendering::GraphicsPipelineCreateInfo& createInfo) override;
        Ref<Rendering::ComputePipeline> CreateComputePipeline(const Rendering::ComputePipelineCreateInfo& createInfo) override;
        Ref<Rendering::Shader> CreateShader(const Rendering::ShaderCreateInfo& createInfo) override;

        VkInstance GetInstance() const;
        VkDevice GetHandle() const;
        VmaAllocator GetAllocator() const;
        VkPhysicalDevice GetPhysicalDevice() const;
        Ref<Surface> GetSurface();
        Swapchain* GetSwapchain();
        CommandPool* GetCommandPool();
        CommandPool* GetTransferCommandPool();
        Queue* GetGraphicsQueue();
        Queue* GetPresentQueue();
        Queue* GetComputeQueue();
        Queue* GetTransferQueue();
        DescriptorPool* GetDescriptorPool();

        Semaphore& GetCurrentSubmitSemaphore();
        Semaphore& GetCurrentPresentSemaphore();
        Fence& GetCurrentFence();
        CommandBuffer& GetCurrentCommandBuffer();

        uint32_t GetCurrentFrameIndex() const;
    private:
        VkInstance m_Instance = nullptr;
        VkPhysicalDevice m_PhysicalDevice = nullptr;
        VkDevice m_Handle = nullptr;
        VmaAllocator m_Allocator = nullptr;
        Ref<Surface> m_Surface = nullptr;
        Swapchain m_Swapchain;
        CommandPool m_CommandPool;
        CommandPool m_TransferCommandPool;
        DescriptorPool m_DescriptorPool;

        Queue m_GraphicsQueue;
        Queue m_PresentQueue;
        Queue m_ComputeQueue;
        Queue m_TransferQueue;
        Frame m_Frames[3];

        uint32_t m_CurrentFrameIndex = 0;
        uint32_t m_LastFrameIndex = 0;

#if defined(NOVA_DEV) || defined(NOVA_DEBUG)
        VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
#endif
    };
}