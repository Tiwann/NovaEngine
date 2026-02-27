#pragma once
#include "Rendering/RenderTarget.h"
#include "Containers/Lazy.h"
#include "Texture.h"

typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;
typedef struct VmaAllocation_T* VmaAllocation;

namespace Nova::Vulkan
{
    class RenderTarget final : public Nova::RenderTarget
    {
    public:
        bool Initialize(const RenderTargetCreateInfo& createInfo) override;
        void Destroy() override;

        bool Resize(uint32_t newWidth, uint32_t newHeight) override;
        Ref<Nova::Texture> GetColorTexture() override;
        Ref<Nova::Texture> GetDepthTexture() override;
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
        Lazy<Ref<Texture>> m_ColorTexture;
        Lazy<Ref<Texture>> m_DepthTexture;
    };
}
