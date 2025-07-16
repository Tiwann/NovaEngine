#pragma once
#include "Rendering/Texture.h"

typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;
typedef struct VmaAllocation_T* VmaAllocation;

namespace Nova::Vulkan
{
    class Device;

    class Texture : public Rendering::Texture
    {
    public:
        Texture() = default;
        ~Texture() override = default;

        bool Initialize(const Rendering::TextureCreateInfo& createInfo) override;
        void Destroy() override;
        bool IsValid() override;

    private:
        Device* m_Device = nullptr;
        VkImage m_Image = nullptr;
        VkImageView m_ImageView = nullptr;
        VmaAllocation m_Allocation = nullptr;
    };
}
