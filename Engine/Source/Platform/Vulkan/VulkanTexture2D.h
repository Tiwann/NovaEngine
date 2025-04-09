#pragma once
#include "Graphics/Texture2D.h"
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

namespace Nova
{
    class VulkanRenderer;
    
    class VulkanTexture2D : public Texture2D
    {
    public:
        VulkanTexture2D(const String& Name, u32 Width, u32 Height, const TextureParams& Params, u32 Slot);

        void SetTextureParameters(const TextureParams& Params) override;
        void SetData(u8* Data, u32 Width, u32 Height, Format Format) override;
        SharedPtr<Image> GetImage() const override;
        void Bind() const override;
        void Unbind() const override;
        uintptr_t GetHandle() const override;
        bool GetPixels(Buffer<u8>& OutPixels) const override;

    private:
        VkImage m_Handle = nullptr;
        VkImageView m_ImageView = nullptr;
        VkSampler m_Sampler = nullptr;
        VmaAllocation m_Allocation = nullptr;
        VmaAllocationInfo m_AllocationInfo = {};

        static VkFormat ConvertFormat(Format Format);
        static VkFilter ConvertFilter(TextureFilter Filter);
        static VkSamplerAddressMode ConvertWrap(TextureWrap Wrap);
    };
}
