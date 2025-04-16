#pragma once
#include "Rendering/Texture2D.h"
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

namespace Nova
{
    class VulkanRenderer;
    
    class VulkanTexture2D final : public Texture2D
    {
    public:
        VulkanTexture2D(const String& Name, u32 Width, u32 Height, const TextureParams& Params, u32 Slot);

        void SetTextureParameters(const TextureParams& Params) override;
        void SetData(u8* Data, u32 Width, u32 Height, const Format& Format) override;
        void Bind() const override;
        void Unbind() const override;
        uintptr_t GetHandle() const override;
    private:
        VkImage m_Handle = nullptr;
        VkImageView m_ImageView = nullptr;
        VmaAllocation m_Allocation = nullptr;
        VmaAllocationInfo m_AllocationInfo = {};
    };
}
