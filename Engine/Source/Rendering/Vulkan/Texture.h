#pragma once
#include "Rendering/Texture.h"

typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;
typedef struct VmaAllocation_T* VmaAllocation;

namespace Nova::Vulkan
{
    class Device;
    class Swapchain;
    class RenderTarget;

    class Texture final : public Nova::Texture
    {
    public:
        Texture() = default;
        ~Texture() override = default;

        bool Initialize(const TextureCreateInfo& createInfo) override;
        void Destroy() override;
        bool IsValid() override;

        VkImage GetImage() const;
        VkImageView GetImageView() const;
        VmaAllocation GetAllocation() const;
        uint32_t GetImageLayout() const;

        Array<uint8_t> GetPixels() override;
    private:
        friend Swapchain;
        friend RenderTarget;

        Device* m_Device = nullptr;
        VkImage m_Image = nullptr;
        VkImageView m_ImageView = nullptr;
        VmaAllocation m_Allocation = nullptr;
        uint32_t m_ImageLayout = 0;
    };
}
