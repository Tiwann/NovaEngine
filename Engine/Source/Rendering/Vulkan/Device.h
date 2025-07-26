#pragma once
#include "Rendering/Device.h"
#include "Queue.h"
#include "Surface.h"
#include "Swapchain.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "Fence.h"
#include "Semaphore.h"

typedef struct VkInstance_T* VkInstance;
typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
typedef struct VkDevice_T* VkDevice;
typedef struct VmaAllocator_T* VmaAllocator;

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
        void WaitIdle() override;

        void ResolveToSwapchain(Rendering::RenderTarget& renderTarget) override;
        void BlitToSwapchain(Rendering::RenderTarget& renderTarget, Filter filter) override;
        void BlitToSwapchain(Rendering::Texture& texture, Filter filter) override;
        void BlitToRenderTarget(Rendering::Texture& srcTexture, Rendering::RenderTarget& destRenderTarget, uint32_t x, uint32_t y) override;

        VkInstance GetInstance() const;
        VkDevice GetHandle() const;
        VmaAllocator GetAllocator() const;
        VkPhysicalDevice GetPhysicalDevice() const;
        Surface* GetSurface();
        Swapchain* GetSwapchain();
        CommandPool* GetCommandPool();
        Queue* GetGraphicsQueue();
        Queue* GetPresentQueue();
        Queue* GetComputeQueue();
        Queue* GetTransferQueue();

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
        Surface m_Surface;
        Swapchain m_Swapchain;
        CommandPool m_CommandPool;

        Queue m_GraphicsQueue;
        Queue m_PresentQueue;
        Queue m_ComputeQueue;
        Queue m_TransferQueue;
        Frame m_Frames[3];

        uint32_t m_CurrentFrameIndex = 0;
        uint32_t m_LastFrameIndex = 0;
    };
}