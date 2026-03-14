#pragma once
#include "Fence.h"
#include "Texture.h"
#include "TextureView.h"
#include "Containers/Lazy.h"
#include "Rendering/Swapchain.h"

typedef struct VkSwapchainKHR_T* VkSwapchainKHR;
typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;

namespace Nova::Vulkan
{
    class Semaphore;
    class RenderDevice;
    class RenderTarget;
    class CommandBuffer;

    class Swapchain final : public Nova::Swapchain
    {
    public:
        bool Initialize(const SwapchainCreateInfo& createInfo) override;
        void Destroy() override;
        bool Recreate() override;
        void SetName(StringView name) override;

        bool AcquireNextImage(const Semaphore* semaphore, const Fence* fence, uint32_t& frameIndex) const;

        const Nova::Texture* GetTexture() override;
        const Nova::TextureView* GetTextureView() override;

        VkSwapchainKHR GetHandle() const;
        const VkSwapchainKHR* GetHandlePtr() const;
        VkImage GetImage(uint32_t index) const;
        VkImageView GetImageView(uint32_t index) const;
    private:
        VkSwapchainKHR m_Handle = nullptr;
        VkImage m_Images[3] = { nullptr, nullptr, nullptr };
        VkImageView m_ImageViews[3] = { nullptr, nullptr, nullptr };
        Lazy<Texture> m_Textures[3];
        Lazy<TextureView> m_TextureViews[3];
    };
}