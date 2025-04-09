#include "VulkanTexture2D.h"
#include "Runtime/Application.h"
#include "VulkanRenderer.h"

namespace Nova
{
    VulkanTexture2D::VulkanTexture2D(const String& Name, u32 Width, u32 Height, const TextureParams& Params, u32 Slot): Texture2D(Name, Width, Height, Params, Slot)
    {
        if (Width == 0 || Height == 0)
        {
            return;
        }
        const VulkanRenderer* Renderer = g_Application->GetRenderer<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        const VmaAllocator Allocator = Renderer->GetAllocator();
        
        VkImageCreateInfo ImageCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        ImageCreateInfo.format = ConvertFormat(Params.Format);
        ImageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
        ImageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
        ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        ImageCreateInfo.arrayLayers = 1;
        ImageCreateInfo.mipLevels = 1;
        ImageCreateInfo.extent.width = Width;
        ImageCreateInfo.extent.height = Height;
        ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;


        VmaAllocationCreateInfo AllocationCreateInfo = { };
        AllocationCreateInfo.priority = 1.0f;
        AllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        AllocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        
        if (VK_FAILED(vmaCreateImage(Allocator,
            &ImageCreateInfo,
            &AllocationCreateInfo,
            &m_Handle,
            &m_Allocation,
            &m_AllocationInfo)))
        {
            NOVA_VULKAN_ERROR("Failed to create Vulkan image!");
            return;
        }

        VkImageViewCreateInfo ImageViewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        ImageViewCreateInfo.image = m_Handle;
        ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ImageViewCreateInfo.format = ConvertFormat(Params.Format);
        ImageViewCreateInfo.components = VkComponentMapping{ VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
        ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        ImageViewCreateInfo.subresourceRange.levelCount = 1;
        ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        ImageViewCreateInfo.subresourceRange.layerCount = 1;
        if (VK_FAILED(vkCreateImageView(Device, &ImageViewCreateInfo, nullptr, &m_ImageView)))
        {
            NOVA_VULKAN_ERROR("Failed to create Vulkan image view (Texture2D)!");
            vmaDestroyImage(Allocator, m_Handle, m_Allocation);
            return;
        }

        VkSamplerCreateInfo SamplerCreateInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        SamplerCreateInfo.magFilter = ConvertFilter(Params.Filter);
        SamplerCreateInfo.minFilter = ConvertFilter(Params.Filter);
        SamplerCreateInfo.addressModeU = ConvertWrap(Params.Wrap);
        SamplerCreateInfo.addressModeV = ConvertWrap(Params.Wrap);
        SamplerCreateInfo.addressModeW = ConvertWrap(Params.Wrap);
        SamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        SamplerCreateInfo.anisotropyEnable = true;
        SamplerCreateInfo.unnormalizedCoordinates = false;

        if (VK_FAILED(vkCreateSampler(Device, &SamplerCreateInfo, nullptr, &m_Sampler)))
        {
            NOVA_VULKAN_ERROR("Failed to create Vulkan Sampler !");
            vmaDestroyImage(Allocator, m_Handle, m_Allocation);
            return;
        }
    }

    void VulkanTexture2D::SetTextureParameters(const TextureParams& Params)
    {
        if (m_Params != Params)
        {
            m_Params = Params;
        }
    }

    void VulkanTexture2D::SetData(u8* Data, u32 Width, u32 Height, Format Format)
    {
        const VulkanRenderer* Renderer = g_Application->GetRenderer<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        const VmaAllocator Allocator = Renderer->GetAllocator();

        if (Width != m_Width || Height != m_Height)
        {
            vkDestroyImageView(Device, m_ImageView, nullptr);
            vkDestroySampler(Device, m_Sampler, nullptr);
            vmaDestroyImage(Allocator, m_Handle, m_Allocation);
            vkDestroyImageView(Device, m_ImageView, nullptr);

            VkImageCreateInfo ImageCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
            ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
            ImageCreateInfo.format = ConvertFormat(Format);
            ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            ImageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            ImageCreateInfo.arrayLayers = 1;
            ImageCreateInfo.mipLevels = 1;
            ImageCreateInfo.extent.width = Width;
            ImageCreateInfo.extent.height = Height;
            ImageCreateInfo.extent.depth = 1;
            ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;


            VmaAllocationCreateInfo AllocationCreateInfo = {};
            AllocationCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
            AllocationCreateInfo.priority = 1.0f;
            AllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            AllocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

            VkResult Result = vmaCreateImage(Allocator,
                &ImageCreateInfo,
                &AllocationCreateInfo,
                &m_Handle,
                &m_Allocation,
                &m_AllocationInfo);
            if (VK_FAILED(Result))
            {
                NOVA_VULKAN_ERROR("Failed to create Vulkan image!");
                return;
            }

            VkImageViewCreateInfo ImageViewCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
            ImageViewCreateInfo.image = m_Handle;
            ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            ImageViewCreateInfo.format = ConvertFormat(Format);
            ImageViewCreateInfo.components = VkComponentMapping{
                VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY
            };
            
            ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            ImageViewCreateInfo.subresourceRange.levelCount = 1;
            ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            ImageViewCreateInfo.subresourceRange.layerCount = 1;
            if (VK_FAILED(vkCreateImageView(Device, &ImageViewCreateInfo, nullptr, &m_ImageView)))
            {
                NOVA_VULKAN_ERROR("Failed to create Vulkan image view (Texture2D)!");
                vmaDestroyImage(Allocator, m_Handle, m_Allocation);
                return;
            }

            VkSamplerCreateInfo SamplerCreateInfo = {VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
            SamplerCreateInfo.magFilter = ConvertFilter(m_Params.Filter);
            SamplerCreateInfo.minFilter = ConvertFilter(m_Params.Filter);
            SamplerCreateInfo.addressModeU = ConvertWrap(m_Params.Wrap);
            SamplerCreateInfo.addressModeV = ConvertWrap(m_Params.Wrap);
            SamplerCreateInfo.addressModeW = ConvertWrap(m_Params.Wrap);
            SamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            SamplerCreateInfo.anisotropyEnable = true;
            SamplerCreateInfo.maxAnisotropy = 1.0f;
            SamplerCreateInfo.unnormalizedCoordinates = false;

            if (VK_FAILED(vkCreateSampler(Device, &SamplerCreateInfo, nullptr, &m_Sampler)))
            {
                NOVA_VULKAN_ERROR("Failed to create Vulkan Sampler !");
                vmaDestroyImage(Allocator, m_Handle, m_Allocation);
                return;
            }
        }
        
        if (VK_FAILED(vmaCopyMemoryToAllocation(Allocator, Data, m_Allocation, 0, (size_t)(Width * Height) * GetBytesPerPixel(Format))))
        {
            NOVA_VULKAN_ERROR("Failed to copy texture data to gpu memory!");
        }
    }
    
    SharedPtr<Image> VulkanTexture2D::GetImage() const
    {
        return nullptr;
    }

    void VulkanTexture2D::Bind() const
    {
        
    }

    void VulkanTexture2D::Unbind() const
    {
    }

    uintptr_t VulkanTexture2D::GetHandle() const
    {
        return (uintptr_t)(VkImage)m_Handle;
    }

    bool VulkanTexture2D::GetPixels(Buffer<u8>& OutPixels) const
    {
        return false;
    }

    VkFormat VulkanTexture2D::ConvertFormat(Format Format)
    {
        switch (Format)
        {
        case Format::RGBA8: return VK_FORMAT_R8G8B8A8_UNORM;
        case Format::RGBA16: return VK_FORMAT_R16G16B16A16_UNORM;
        case Format::RGBA32F: return VK_FORMAT_R32G32B32A32_SFLOAT;
        }
        return VK_FORMAT_UNDEFINED;
    }

    VkFilter VulkanTexture2D::ConvertFilter(TextureFilter Filter)
    {
        switch (Filter) {
        case TextureFilter::Nearest: return VK_FILTER_NEAREST;
        case TextureFilter::Linear: return VK_FILTER_LINEAR;
        }
        throw;
    }

    VkSamplerAddressMode VulkanTexture2D::ConvertWrap(TextureWrap Wrap)
    {
        switch (Wrap) {
        case TextureWrap::Clamp: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case TextureWrap::Repeat: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case TextureWrap::Mirror: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        }
        throw;
    }
}
