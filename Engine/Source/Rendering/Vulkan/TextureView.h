#pragma once
#include "Rendering/TextureView.h"

typedef struct VkImageView_T* VkImageView;
typedef struct VkImage_T* VkImage;

namespace Nova::Vulkan
{
    class TextureView final : public Nova::TextureView
    {
    public:
        bool Initialize(const TextureViewCreateInfo& createInfo) override;
        void Destroy() override;

        VkImage GetImage() const;
        VkImageView GetHandle() const;

    private:
        VkImageView m_Handle = nullptr;
    };
}