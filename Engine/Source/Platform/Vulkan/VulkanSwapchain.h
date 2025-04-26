#pragma once
#include "Rendering/Swapchain.h"

typedef struct VkSwapchainKHR_T* VkSwapchainKHR;

namespace Nova
{
    class VulkanSwapchain : public Swapchain
    {
    public:
        explicit VulkanSwapchain(Renderer* Owner) : Swapchain(Owner) {};

        bool Initialize(const SwapchainDescription& Description) override;
        bool Resize(const i32 Width, const i32 Height) override;
        void Destroy() override;

        VkSwapchainKHR GetHandle() const { return m_Handle;}

    private:
        VkSwapchainKHR m_Handle = nullptr;
    };
}
