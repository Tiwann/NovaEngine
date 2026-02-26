#pragma once
#include "Rendering/Texture.h"

typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;
typedef struct VmaAllocation_T* VmaAllocation;

namespace Nova::Vulkan
{
    class RenderDevice;
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
        Array<uint8_t> GetPixels() override;
    private:
        friend Swapchain;
        friend RenderTarget;

        RenderDevice* m_Device = nullptr;
        VkImage m_Image = nullptr;
        VkImageView m_ImageView = nullptr;
        VmaAllocation m_Allocation = nullptr;
    };
}
