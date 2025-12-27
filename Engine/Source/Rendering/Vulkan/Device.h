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
#include "Rendering/SamplerManager.h"

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

    class Device final : public Nova::Device
    {
    public:
        bool Initialize(const DeviceCreateInfo& createInfo) override;
        void Destroy() override;

        bool BeginFrame() override;
        void EndFrame() override;
        void Present() override;
        void WaitIdle() const override;
        void SetName(StringView name) override;
        DeviceType GetDeviceType() override;
        uint32_t GetImageCount() const override;

        Ref<Nova::RenderTarget> CreateRenderTarget(const RenderTargetCreateInfo& createInfo) override;
        Ref<Nova::Surface> CreateSurface(const SurfaceCreateInfo& createInfo) override;
        Ref<Nova::Texture> CreateTexture(const TextureCreateInfo& createInfo) override;
        Ref<Nova::Texture> CreateTextureUnitialized() override;
        Ref<Nova::Sampler> CreateSampler(const SamplerCreateInfo& createInfo) override;
        Ref<Nova::Sampler> GetOrCreateSampler(const SamplerCreateInfo& createInfo) override;
        Ref<Nova::Buffer> CreateBuffer(const BufferCreateInfo& createInfo) override;
        Ref<Nova::GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo) override;
        Ref<Nova::ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) override;
        Ref<Nova::Shader> CreateShader(const ShaderCreateInfo& createInfo) override;
        Ref<Nova::Material> CreateMaterial(const MaterialCreateInfo& createInfo) override;
        Ref<Nova::Fence> CreateFence(const FenceCreateInfo& createInfo) override;

        VkInstance GetInstance() const;
        VkDevice GetHandle() const;
        VmaAllocator GetAllocator() const;
        VkPhysicalDevice GetPhysicalDevice() const;
        Ref<Nova::Surface> GetSurface();
        Nova::Swapchain* GetSwapchain() override;
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
        Nova::CommandBuffer* GetCurrentCommandBuffer() override;


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
        SamplerManager m_SamplerManager;

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