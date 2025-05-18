#pragma once
#include "Rendering/Swapchain.h"

typedef struct VkSwapchainKHR_T* VkSwapchainKHR;
typedef struct VkImage_T* VkImage;

namespace Nova
{
    class VulkanSwapchain : public Swapchain
    {
    public:
        explicit VulkanSwapchain(Renderer* Owner) : Swapchain(Owner) {};

        bool Initialize(const SwapchainCreateInfo& CreateInfo) override;
        void SetDebugName(const String& Name) override;
        bool Recreate() override;
        void Destroy() override;

        void GetImages(VkImage* OutImages) const;

        VkSwapchainKHR GetHandle() const { return m_Handle; }
        const VkSwapchainKHR* GetHandlePtr() const { return &m_Handle; }
    private:
        VkSwapchainKHR m_Handle = nullptr;
    };
}
