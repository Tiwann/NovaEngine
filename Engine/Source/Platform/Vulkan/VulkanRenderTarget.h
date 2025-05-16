#pragma once
#include "Rendering/RenderTarget.h"

typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;
typedef struct VmaAllocation_T* VmaAllocation;

namespace Nova
{
    class Renderer;

    struct VulkanRenderTargetAttachment : RenderTargetAttachment
    {
        VkImage Images[3] { };
        VkImageView ImageViews[3] { };
        VmaAllocation Allocations[3] { };
    };

    class VulkanRenderTarget : public RenderTarget
    {
    public:
        explicit VulkanRenderTarget(Renderer* Owner);
        bool Initialize(const RenderTargetCreateInfo& CreateInfo) override;
        bool Resize(const u32 Width, const u32 Height) override;
        void Destroy() override;
        void BeginRendering() override;
        void EndRendering() override;

    private:
        RenderTargetCreateInfo m_CachedCreateInfo;
    };
}
