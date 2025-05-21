#pragma once
#include "Rendering/CommandBuffer.h"
#include "Rendering/RenderTarget.h"

typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;
typedef struct VmaAllocation_T* VmaAllocation;

namespace Nova
{
    class Renderer;

    class VulkanRenderTarget : public RenderTarget
    {
    public:
        explicit VulkanRenderTarget(Renderer* Owner);
        bool Initialize(const RenderTargetCreateInfo& CreateInfo) override;
        bool Resize(u32 Width, u32 Height) override;
        void Destroy() override;
        void BeginRendering(CommandBuffer* Cmd) const;
        void EndRendering(CommandBuffer* Cmd) const;

        VkImage GetColorImage() const;
        VkImageView GetColorImageView() const;
        VkImage GetDepthImage() const;
        VkImageView GetDepthImageView() const;
    private:
        RenderTargetCreateInfo m_CachedCreateInfo;
        VkImage m_ColorImages[3] { };
        VkImageView m_ColorImageViews[3] { };
        VmaAllocation m_ColorAllocations[3] { };
        VkImage m_DepthImages[3] { };
        VkImageView m_DepthImageViews[3] { };
        VmaAllocation m_DepthAllocations[3] { };
    };
}
