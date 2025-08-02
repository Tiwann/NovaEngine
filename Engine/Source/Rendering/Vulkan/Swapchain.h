#pragma once
#include "Fence.h"
#include "Texture.h"
#include "Containers/Lazy.h"
#include "Rendering/Swapchain.h"

typedef struct VkSwapchainKHR_T* VkSwapchainKHR;
typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;

namespace Nova::Vulkan
{
    class Semaphore;
    class Device;
    class RenderTarget;
    class CommandBuffer;

    class Swapchain : public Rendering::Swapchain
    {
    public:
        bool Initialize(const Rendering::SwapchainCreateInfo& createInfo) override;
        void Destroy() override;
        bool Recreate() override;

        bool AcquireNextImage(const Semaphore* semaphore, const Fence* fence, uint32_t& frameIndex);
        void ResolveImage(const CommandBuffer* commandBuffer, const RenderTarget* renderTarget);

        const Texture& GetTexture(uint32_t index);
        const Texture& GetCurrentTexture();

        VkSwapchainKHR GetHandle() const;
        const VkSwapchainKHR* GetHandlePtr() const;
        VkImage GetImage(uint32_t index) const;
        VkImageView GetImageView(uint32_t index) const;
    private:
        VkSwapchainKHR m_Handle = nullptr;
        VkImage m_Images[3] = { nullptr, nullptr, nullptr };
        VkImageView m_ImageViews[3] = { nullptr, nullptr, nullptr };
        Lazy<Texture> m_Textures[3];
    };
}