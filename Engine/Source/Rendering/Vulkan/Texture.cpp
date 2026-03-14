#include "Texture.h"
#include "TextureView.h"
#include "RenderDevice.h"
#include "Conversions.h"
#include "Buffer.h"
#include "Runtime/Log.h"
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

namespace Nova::Vulkan
{
    static TextureDimension GetTextureDimension(const uint32_t width, const uint32_t height, const uint32_t depth)
    {
        TextureDimension result = TextureDimension::None;
        if (width > 1) result = TextureDimension::Dim1D;
        if (height > 1) result = TextureDimension::Dim2D;
        if (depth > 1) result = TextureDimension::Dim3D;
        return result;
    }
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

        const TextureDimension dimension = GetTextureDimension(createInfo.width, createInfo.height, createInfo.depth);

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
        const AccessFlagBits destAccess = isColorAttachment ? AccessFlagBits::ColorAttachmentWrite :
        isDepthAttachment ? AccessFlagBits::DepthStencilAttachmentWrite :
        isSampled ? AccessFlagBits::ShaderRead : AccessFlagBits::None;

        const ResourceState destState = isColorAttachment ? ResourceState::ColorAttachment :
        isDepthAttachment ? ResourceState::DepthStencilAttachment :
        isSampled ? ResourceState::ShaderRead : ResourceState::General;

        TextureBarrier barrier;
        barrier.texture = this;
        barrier.sourceAccess = AccessFlagBits::None;
        barrier.destAccess = destAccess;
        barrier.destState = destState;
        barrier.destQueue = nullptr;
        barrier.destQueue = nullptr;
        RenderDevice::ImmediateTextureBarrier(barrier);

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

    Array<uint8_t> Texture::GetPixels()
    {
        if (!m_Device) return {};

        Fence fence;
        if (!fence.Initialize({m_Device, FenceCreateFlagBits::None})) return {};

        CommandPool* commandPool = m_Device->GetCommandPool();
        CommandBuffer commandBuffer = commandPool->AllocateCommandBuffer(CommandBufferLevel::Primary);

        const size_t size = m_Width * m_Height * GetFormatComponentCount(m_Format) * GetFormatBytesPerChannel(m_Format);
        BufferCreateInfo createInfo;
        createInfo.size = size;
        createInfo.usage = BufferUsage::StagingBuffer;
        Ref<Buffer> buffer = m_Device->CreateBuffer(createInfo);

        if (commandBuffer.Begin({CommandBufferUsageFlagBits::OneTimeSubmit}))
        {

            VkImageMemoryBarrier toTransferBarrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
            toTransferBarrier.image = m_Image;
            toTransferBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            toTransferBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            toTransferBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            toTransferBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            toTransferBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            toTransferBarrier.subresourceRange.baseMipLevel = 0;
            toTransferBarrier.subresourceRange.levelCount = m_Mips;
            toTransferBarrier.subresourceRange.baseArrayLayer = 0;
            toTransferBarrier.subresourceRange.layerCount = 1;
            toTransferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            toTransferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            vkCmdPipelineBarrier(commandBuffer.GetHandle(), VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &toTransferBarrier);


            VkBufferImageCopy imageCopyRegion{};
            imageCopyRegion.bufferOffset = 0;
            imageCopyRegion.bufferRowLength = 0;
            imageCopyRegion.bufferImageHeight = 0;
            imageCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageCopyRegion.imageSubresource.mipLevel = 0;
            imageCopyRegion.imageSubresource.baseArrayLayer = 0;
            imageCopyRegion.imageSubresource.layerCount = 1;
            imageCopyRegion.imageOffset = {0, 0, 0};
            imageCopyRegion.imageExtent = {m_Width, m_Height, 1};
            vkCmdCopyImageToBuffer(commandBuffer.GetHandle(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer->GetHandle(), 1, &imageCopyRegion);

            VkImageMemoryBarrier toReadBarrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
            toReadBarrier.image = m_Image;
            toReadBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            toReadBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            toReadBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            toReadBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            toReadBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            toReadBarrier.subresourceRange.baseMipLevel = 0;
            toReadBarrier.subresourceRange.levelCount = m_Mips;
            toReadBarrier.subresourceRange.baseArrayLayer = 0;
            toReadBarrier.subresourceRange.layerCount = 1;
            toReadBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            toReadBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            vkCmdPipelineBarrier(commandBuffer.GetHandle(), VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &toReadBarrier);

            commandBuffer.End();
        }
        else return {};

        Queue* graphicsQueue = m_Device->GetGraphicsQueue();
        graphicsQueue->Submit(&commandBuffer, nullptr, nullptr, &fence);
        fence.Wait(~0);
        fence.Destroy();
        commandBuffer.Free();

        Array<uint8_t> result(size);
        if (!buffer->CopyDataTo(0, size, result.Data()))
            return {};
        return result;
    }
}
