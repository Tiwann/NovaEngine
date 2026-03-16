#include "Texture.h"
#include "TextureView.h"
#include "RenderDevice.h"
#include "Conversions.h"
#include "Buffer.h"
#include "Runtime/Log.h"
#include "Utils/TextureUtils.h"
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>


namespace Nova::Vulkan
{
    bool Texture::Initialize(const TextureCreateInfo& createInfo)
    {
        if (createInfo.format == Format::None) return false;
        if (createInfo.sampleCount <= 0) return false;
        if (createInfo.mipCount <= 0) return false;
        if (createInfo.width <= 0 || createInfo.height <= 0) return false;
        if (createInfo.arrayCount <= 0) return false;
        if (createInfo.depth == 0) return false;

        RenderDevice* device = static_cast<RenderDevice*>(createInfo.device);
        const VmaAllocator allocatorHandle = device->GetAllocator();
        vmaDestroyImage(allocatorHandle, m_Image, m_Allocation);

        const TextureDimension dimension = TextureUtils::GetTextureDimension(createInfo.width, createInfo.height, createInfo.depth);

        VkImageCreateInfo imageCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imageCreateInfo.imageType = Convert<VkImageType>(dimension);
        imageCreateInfo.format = Convert<VkFormat>(createInfo.format);
        imageCreateInfo.extent.width = createInfo.width;
        imageCreateInfo.extent.height = createInfo.height;
        imageCreateInfo.extent.depth = createInfo.depth;
        imageCreateInfo.mipLevels = createInfo.mipCount;
        imageCreateInfo.arrayLayers = createInfo.arrayCount;
        imageCreateInfo.samples = (VkSampleCountFlagBits)createInfo.sampleCount;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage = Convert<VkImageUsageFlags>(createInfo.usageFlags);
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo allocationCreateInfo = { };
        allocationCreateInfo.priority = 1.0f;
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

        const VkResult result = vmaCreateImage(allocatorHandle,
           &imageCreateInfo,
           &allocationCreateInfo,
           &m_Image,
           &m_Allocation,
           nullptr);
        if (result != VK_SUCCESS)
            return false;

        const auto& usageFlags = createInfo.usageFlags;
        const bool isColorAttachment = usageFlags & TextureUsageFlagBits::ColorAttachment;
        const bool isDepthAttachment = usageFlags & TextureUsageFlagBits::DepthStencilAttachment;
        const bool isSampled = usageFlags & TextureUsageFlagBits::Sampled;

        TextureAspectFlags aspectFlags = TextureAspectFlags::None();
        if (isColorAttachment || isSampled) aspectFlags |= TextureAspectFlagBits::Color;
        if (isDepthAttachment)
        {
            aspectFlags |= TextureAspectFlagBits::Depth;
            aspectFlags |= TextureAspectFlagBits::Stencil;
        }

        m_Device = device;
        m_Format = createInfo.format;
        m_Width = createInfo.width;
        m_Height = createInfo.height;
        m_Depth = createInfo.depth;
        m_Mips = createInfo.mipCount;
        m_ArrayCount = createInfo.arrayCount;
        m_SampleCount = createInfo.sampleCount;
        m_UsageFlags = createInfo.usageFlags;
        m_Dimension = dimension;

        TextureViewCreateInfo tvCreateInfo;
        tvCreateInfo.device = device;
        tvCreateInfo.texture = this;
        tvCreateInfo.width = createInfo.width;
        tvCreateInfo.height = createInfo.height;
        tvCreateInfo.depth = createInfo.depth;
        tvCreateInfo.format = createInfo.format;
        tvCreateInfo.baseMipLevel = 0;
        tvCreateInfo.mipCount = createInfo.mipCount;
        tvCreateInfo.baseArray = 0;
        tvCreateInfo.arrayCount = createInfo.arrayCount;
        tvCreateInfo.aspectFlags = aspectFlags;
        if (m_View)
        {
            if (!m_View->Initialize(tvCreateInfo))
                return false;
        } else
        {
            m_View = device->CreateTextureView(tvCreateInfo);
            if (!m_View) return false;
        }

        // DECIDED TO EXPLICITLY TRANSITION TO LAYOUT GENERAL BY DEFAULT
        const ResourceAccessFlagBits destAccess = isColorAttachment ? ResourceAccessFlagBits::ColorAttachmentWrite :
        isDepthAttachment ? ResourceAccessFlagBits::DepthStencilAttachmentWrite :
        isSampled ? ResourceAccessFlagBits::ShaderRead : ResourceAccessFlagBits::None;

        const ResourceState destState = isColorAttachment ? ResourceState::ColorAttachment :
        isDepthAttachment ? ResourceState::DepthStencilAttachment :
        isSampled ? ResourceState::ShaderRead : ResourceState::General;

        TextureBarrier barrier;
        barrier.texture = this;
        barrier.sourceAccess = ResourceAccessFlagBits::None;
        barrier.destAccess = destAccess;
        barrier.destState = destState;
        barrier.destQueue = nullptr;
        barrier.destQueue = nullptr;
        RenderDevice::ImmediateTextureBarrier(device, barrier);
        return true;
    }

    void Texture::Destroy()
    {
        if (m_View) m_View->Destroy();
        const VmaAllocator allocatorHandle = m_Device->GetAllocator();
        vmaDestroyImage(allocatorHandle, m_Image, m_Allocation);
        m_Device = nullptr;
        m_Image = nullptr;
    }

    bool Texture::IsValid()
    {
        return m_Device && m_Image && m_Allocation;
    }

    VkImage Texture::GetImage() const
    {
        return m_Image;
    }

    VmaAllocation Texture::GetAllocation() const
    {
        return m_Allocation;
    }
}
