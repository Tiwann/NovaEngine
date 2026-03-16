#include "VulkanUtils.h"
#include "Conversions.h"
#include "Texture.h"
#include "Queue.h"
#include "Buffer.h"
#include "Rendering/ResourceBarrier.h"

namespace Nova::Vulkan
{
    VkPipelineStageFlags GetSourcePipelineStageFlags(const ResourceAccessFlags accessFlags)
    {
        VkPipelineStageFlags flags = 0;

        if (accessFlags & ResourceAccessFlagBits::ShaderRead ||
            accessFlags & ResourceAccessFlagBits::ShaderWrite)
        {
            flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        }

        if (accessFlags & ResourceAccessFlagBits::ColorAttachmentRead ||
            accessFlags & ResourceAccessFlagBits::ColorAttachmentWrite)
        {
            flags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }

        if (accessFlags & ResourceAccessFlagBits::DepthStencilAttachmentRead ||
            accessFlags & ResourceAccessFlagBits::DepthStencilAttachmentWrite)
        {
            flags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        }

        if (accessFlags & ResourceAccessFlagBits::TransferRead ||
            accessFlags & ResourceAccessFlagBits::TransferWrite)
        {
            flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
        }

        if (accessFlags & ResourceAccessFlagBits::HostRead ||
            accessFlags & ResourceAccessFlagBits::HostWrite)
        {
            flags |= VK_PIPELINE_STAGE_HOST_BIT;
        }

        if (flags == 0)
            flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

        return flags;
    }

    VkPipelineStageFlags GetDestPipelineStageFlags(const ResourceAccessFlags accessFlags)
    {
        VkPipelineStageFlags flags = 0;

        if (accessFlags & ResourceAccessFlagBits::ShaderRead ||
            accessFlags & ResourceAccessFlagBits::ShaderWrite)
        {
            flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        }

        if (accessFlags & ResourceAccessFlagBits::ColorAttachmentRead ||
            accessFlags & ResourceAccessFlagBits::ColorAttachmentWrite)
        {
            flags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }

        if (accessFlags & ResourceAccessFlagBits::DepthStencilAttachmentRead ||
            accessFlags & ResourceAccessFlagBits::DepthStencilAttachmentWrite)
        {
            flags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        }

        if (accessFlags & ResourceAccessFlagBits::TransferRead ||
            accessFlags & ResourceAccessFlagBits::TransferWrite)
        {
            flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
        }

        if (accessFlags & ResourceAccessFlagBits::HostRead ||
            accessFlags & ResourceAccessFlagBits::HostWrite)
        {
            flags |= VK_PIPELINE_STAGE_HOST_BIT;
        }

        if (flags == 0)
            flags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

        return flags;
    }

    VkImageMemoryBarrier MakeTextureBarrier(const TextureBarrier& barrier)
    {
        const Texture* texture = static_cast<Texture*>(barrier.texture);
        VkImageMemoryBarrier vkBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        vkBarrier.image = texture->GetImage();
        vkBarrier.oldLayout = Convert<VkImageLayout>(texture->GetState());
        vkBarrier.newLayout = Convert<VkImageLayout>(barrier.destState);
        vkBarrier.srcAccessMask = Convert<VkAccessFlags>(barrier.sourceAccess);
        vkBarrier.dstAccessMask = Convert<VkAccessFlags>(barrier.destAccess);
        vkBarrier.subresourceRange.aspectMask = Convert(texture->GetFormat());
        vkBarrier.subresourceRange.baseMipLevel = 0;
        vkBarrier.subresourceRange.levelCount = texture->GetMipCount();
        vkBarrier.subresourceRange.baseArrayLayer = 0;
        vkBarrier.subresourceRange.layerCount = texture->GetArrayCount();

        if (barrier.sourceQueue && barrier.destQueue && barrier.sourceQueue != barrier.destQueue)
        {
            const Queue* srcQueue = static_cast<const Queue*>(barrier.sourceQueue);
            const Queue* destQueue = static_cast<const Queue*>(barrier.destQueue);
            vkBarrier.srcQueueFamilyIndex = srcQueue->GetIndex();
            vkBarrier.dstQueueFamilyIndex = destQueue->GetIndex();
        } else
        {
            vkBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            vkBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        }

        return vkBarrier;
    }

    VkBufferMemoryBarrier MakeBufferBarrier(const BufferBarrier& barrier)
    {
        const Buffer* buffer = static_cast<Buffer*>(barrier.buffer);
        VkBufferMemoryBarrier vkBarrier = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
        vkBarrier.buffer = buffer->GetHandle();
        vkBarrier.offset = barrier.offset;
        vkBarrier.size = barrier.size;
        vkBarrier.srcAccessMask = Convert<VkAccessFlags>(barrier.sourceAccess);
        vkBarrier.dstAccessMask = Convert<VkAccessFlags>(barrier.destAccess);
        if (barrier.sourceQueue && barrier.destQueue && barrier.sourceQueue != barrier.destQueue)
        {
            const Queue* srcQueue = static_cast<const Queue*>(barrier.sourceQueue);
            const Queue* destQueue = static_cast<const Queue*>(barrier.destQueue);
            vkBarrier.srcQueueFamilyIndex = srcQueue->GetIndex();
            vkBarrier.dstQueueFamilyIndex = destQueue->GetIndex();
        } else
        {
            vkBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            vkBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        }
        return vkBarrier;
    }
}
