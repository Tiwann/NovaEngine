#pragma once
#include "Fence.h"
#include "Texture.h"
#include "Containers/Lazy.h"
#include "Rendering/Swapchain.h"
#include "Runtime/Ref.h"

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
        void SetName(StringView name) override;

        bool AcquireNextImage(const Semaphore* semaphore, const Fence* fence, uint32_t& frameIndex);

        const Ref<Texture>& GetTexture(uint32_t index);
        const Ref<Texture>& GetCurrentTexture();

        VkSwapchainKHR GetHandle() const;
        const VkSwapchainKHR* GetHandlePtr() const;
        VkImage GetImage(uint32_t index) const;
        VkImageView GetImageView(uint32_t index) const;
    private:
#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        StringView m_Name;
#endif
        VkSwapchainKHR m_Handle = nullptr;
        VkImage m_Images[3] = { nullptr, nullptr, nullptr };
        VkImageView m_ImageViews[3] = { nullptr, nullptr, nullptr };
        Lazy<Ref<Texture>> m_Textures[3] = { Ref<Texture>(nullptr), Ref<Texture>(nullptr), Ref<Texture>(nullptr) };
    };
}