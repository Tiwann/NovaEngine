#pragma once
#include "Containers/StaticArray.h"
#include "Rendering/Swapchain.h"

typedef struct VkSwapchainKHR_T* VkSwapchainKHR;
typedef struct VkImage_T* VkImage;

namespace Nova
{
    class VulkanSwapchain : public Swapchain
    {
    public:
        explicit VulkanSwapchain(Renderer* Owner) : Swapchain(Owner) {}

        bool Initialize(const SwapchainCreateInfo& CreateInfo) override;
        bool Resize(i32 Width, i32 Height) override;
        bool Recreate() override;
        void Destroy() override;

        VkSwapchainKHR GetHandle() const { return m_Handle; }
        const VkSwapchainKHR* GetHandlePtr() const { return &m_Handle; }
        StaticArray<VkImage, 3> GetImages(u32& OutImageCount) const;

    private:
        VkSwapchainKHR m_Handle = nullptr;
        SwapchainCreateInfo m_CachedCreateInfo{};
    };
}
