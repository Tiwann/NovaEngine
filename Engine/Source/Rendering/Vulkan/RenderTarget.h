﻿#pragma once
#include "Rendering/RenderTarget.h"
#include "Containers/Lazy.h"
#include "Texture.h"

typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;
typedef struct VmaAllocation_T* VmaAllocation;

namespace Nova::Vulkan
{
    class CommandBuffer;

    class RenderTarget : public Rendering::RenderTarget
    {
    public:
        bool Initialize(const Rendering::RenderTargetCreateInfo& createInfo) override;
        void Destroy() override;

        bool Resize(uint32_t newWidth, uint32_t newHeight) override;

        const Texture& GetColorTexture();
        const Texture& GetDepthTexture();

        VkImage GetColorImage(size_t index) const;
        VkImageView GetColorImageView(size_t index) const;
        VmaAllocation GetColorImageAllocation(size_t index) const;
        VkImage GetDepthImage(size_t index) const;
        VkImageView GetDepthImageView(size_t index) const;
        VmaAllocation GetDepthImageAllocation(size_t index) const;
    private:
        VkImage m_ColorImages[3] { nullptr, nullptr, nullptr };
        VkImageView m_ColorImageViews[3] { nullptr, nullptr, nullptr };
        VmaAllocation m_ColorAllocations[3] { nullptr, nullptr, nullptr };
        VkImage m_DepthImages[3] { nullptr, nullptr, nullptr };
        VkImageView m_DepthImageViews[3] { nullptr, nullptr, nullptr };
        VmaAllocation m_DepthAllocations[3] { nullptr, nullptr, nullptr };
        CommandBuffer* m_CommandBuffer = nullptr;
        Lazy<Texture> m_ColorTexture;
        Lazy<Texture> m_DepthTexture;
    };
}
