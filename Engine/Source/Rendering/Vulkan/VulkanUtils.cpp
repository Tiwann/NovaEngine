#include "VulkanUtils.h"
#include "Conversions.h"
#include "Texture.h"
#include "Queue.h"
#include "Buffer.h"
#include "Rendering/ResourceBarrier.h"

namespace Nova::Vulkan
{
    VkAccessFlags2 GetSourceAccessFlags(const VkImageLayout layout)
    {
        switch (layout)
        {
        case VK_IMAGE_LAYOUT_UNDEFINED: return VK_ACCESS_2_NONE;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: return VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: return VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: return VK_ACCESS_2_TRANSFER_READ_BIT;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: return VK_ACCESS_2_TRANSFER_WRITE_BIT;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: return VK_ACCESS_2_SHADER_READ_BIT;
        case VK_IMAGE_LAYOUT_GENERAL: return VK_ACCESS_2_NONE;
        default: return VK_ACCESS_2_NONE;
        }
    }

    VkAccessFlags2 GetDestAccessFlags(const VkImageLayout layout)
    {
        switch (layout)
        {
        case VK_IMAGE_LAYOUT_UNDEFINED: return VK_ACCESS_2_NONE;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: return VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: return VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: return VK_ACCESS_2_TRANSFER_WRITE_BIT;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: return VK_ACCESS_2_TRANSFER_READ_BIT;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: return VK_ACCESS_2_SHADER_WRITE_BIT;
        case VK_IMAGE_LAYOUT_GENERAL: return VK_ACCESS_2_NONE;
        default: return VK_ACCESS_2_NONE;
        }
    }

    VkPipelineStageFlags GetPipelineStageFlags(const AccessFlags accessFlags)
    {
        VkPipelineStageFlags stages = 0;

        if (accessFlags & AccessFlagBits::ShaderRead ||
            accessFlags & AccessFlagBits::ShaderWrite)
        {
            stages |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
            stages |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            stages |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        }

        if (accessFlags & AccessFlagBits::ColorAttachmentRead ||
            accessFlags & AccessFlagBits::ColorAttachmentWrite)
        {
            stages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }

        if (accessFlags & AccessFlagBits::DepthStencilAttachmentRead ||
            accessFlags & AccessFlagBits::DepthStencilAttachmentWrite)
        {
            stages |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            stages |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        }

        if (accessFlags & AccessFlagBits::TransferRead ||
            accessFlags & AccessFlagBits::TransferWrite)
        {
            stages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
        }

        if (accessFlags & AccessFlagBits::HostRead ||
            accessFlags & AccessFlagBits::HostWrite)
        {
            stages |= VK_PIPELINE_STAGE_HOST_BIT;
        }

        if (stages == 0)
            stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

        return stages;
    }

    VkImageMemoryBarrier MakeTextureBarrier(const Nova::TextureBarrier& barrier)
    {
        Texture* texture = static_cast<Texture*>(barrier.texture);
        VkImageMemoryBarrier vkBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        vkBarrier.image = texture->GetImage();
        vkBarrier.oldLayout = Convert<VkImageLayout>(texture->GetState());
        vkBarrier.newLayout = Convert<VkImageLayout>(barrier.destState);
        vkBarrier.srcAccessMask = Convert<VkAccessFlags>(barrier.sourceAccess);
        vkBarrier.dstAccessMask = Convert<VkAccessFlags>(barrier.destAccess);
        vkBarrier.subresourceRange.aspectMask = Convert(texture->GetFormat());
        vkBarrier.subresourceRange.baseMipLevel = 0;
        vkBarrier.subresourceRange.levelCount = texture->GetMips();
        vkBarrier.subresourceRange.baseArrayLayer = 0;
        vkBarrier.subresourceRange.layerCount = 1;

        if (barrier.sourceQueue && barrier.destQueue && barrier.sourceQueue != barrier.destQueue)
        {
            const Queue* srcQueue = dynamic_cast<const Queue*>(barrier.sourceQueue);
            const Queue* destQueue = dynamic_cast<const Queue*>(barrier.destQueue);
            vkBarrier.srcQueueFamilyIndex = srcQueue->GetIndex();
            vkBarrier.dstQueueFamilyIndex = destQueue->GetIndex();
        } else
        {
            vkBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            vkBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        }

        return vkBarrier;
    }

    VkBufferMemoryBarrier MakeBufferBarrier(const Nova::BufferBarrier& barrier)
    {
        Buffer* buffer = static_cast<Buffer*>(barrier.buffer);
        VkBufferMemoryBarrier vkBarrier = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
        vkBarrier.buffer = buffer->GetHandle();
        vkBarrier.offset = barrier.offset;
        vkBarrier.size = barrier.size;
        vkBarrier.srcAccessMask = Convert<VkAccessFlags>(barrier.sourceAccess);
        vkBarrier.dstAccessMask = Convert<VkAccessFlags>(barrier.destAccess);
        if (barrier.sourceQueue && barrier.destQueue && barrier.sourceQueue != barrier.destQueue)
        {
            const Queue* srcQueue = dynamic_cast<const Queue*>(barrier.sourceQueue);
            const Queue* destQueue = dynamic_cast<const Queue*>(barrier.destQueue);
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
