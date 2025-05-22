#pragma once
#include "Rendering/Swapchain.h"

typedef struct VkSwapchainKHR_T* VkSwapchainKHR;
typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;

namespace Nova
{
    class VulkanRenderTarget;
    class VulkanCommandBuffer;
    class VulkanSemaphore;

    class VulkanSwapchain : public Swapchain
    {
    public:
        explicit VulkanSwapchain(Renderer* Owner) : Swapchain(Owner) {};

        bool Initialize(const SwapchainCreateInfo& CreateInfo) override;
        void SetDebugName(const String& Name) override;
        bool Recreate() override;
        void Destroy() override;

        VkImage GetImage(size_t FrameIndex) const;
        VkSwapchainKHR GetHandle() const { return m_Handle; }
        const VkSwapchainKHR* GetHandlePtr() const { return &m_Handle; }
        void Resolve(const VulkanCommandBuffer* CommandBuffer, const VulkanRenderTarget* RenderTarget) const;
        u32 AcquireNextImage(const VulkanSemaphore* Semaphore) const;
    private:
        VkSwapchainKHR m_Handle = nullptr;
        VkImage m_Images[3] { };
        VkImageView m_ImageViews[3] { };
    };
}
