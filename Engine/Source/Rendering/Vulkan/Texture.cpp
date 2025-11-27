#include "Texture.h"
#include "Device.h"
#include "Conversions.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>


namespace Nova::Vulkan
{
    bool Texture::Initialize(const TextureCreateInfo& createInfo)
    {
        using namespace Nova;

        if (createInfo.width == 0 || createInfo.height == 0)
            return false;

        if (createInfo.usageFlags == TextureUsageFlagBits::None)
            return false;

        /*if (createInfo.data == nullptr && createInfo.dataSize == 0 && !createInfo.usageFlags.Contains(TextureUsageFlagBits::Storage))
            return false;*/

        Device* device = static_cast<Device*>(createInfo.device);
        const VmaAllocator allocatorHandle = device->GetAllocator();
        const VkDevice deviceHandle = device->GetHandle();

        vmaDestroyImage(allocatorHandle, m_Image, m_Allocation);
        vkDestroyImageView(deviceHandle, m_ImageView, nullptr);

        VkImageType imageType = VK_IMAGE_TYPE_1D;
        if (createInfo.height > 1) imageType = VK_IMAGE_TYPE_2D;
        if (createInfo.depth > 1) imageType = VK_IMAGE_TYPE_3D;
        VkImageCreateInfo imageCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imageCreateInfo.imageType = imageType;
        imageCreateInfo.format = Convert<Format, VkFormat>(createInfo.format);
        imageCreateInfo.extent.width = createInfo.width;
        imageCreateInfo.extent.height = createInfo.height;
        imageCreateInfo.extent.depth = createInfo.depth;
        imageCreateInfo.mipLevels = createInfo.mips;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = (VkSampleCountFlagBits)createInfo.sampleCount;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        if (createInfo.usageFlags.Contains(TextureUsageFlagBits::Sampled))
            imageCreateInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;

        if (createInfo.usageFlags.Contains(TextureUsageFlagBits::Storage))
            imageCreateInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT;


        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo allocationCreateInfo = { };
        allocationCreateInfo.priority = 1.0f;
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        VkResult result = vmaCreateImage(allocatorHandle,
           &imageCreateInfo,
           &allocationCreateInfo,
           &m_Image,
           &m_Allocation,
           nullptr);
        if (result != VK_SUCCESS)
        return false;

        Fence fence;
        if (!fence.Initialize({device, FenceCreateFlagBits::None}))
            return false;

        if (createInfo.data && createInfo.dataSize > 0)
        {
            VkBufferCreateInfo stagingBufferCreateInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
            stagingBufferCreateInfo.size = createInfo.dataSize;
            stagingBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            VmaAllocationCreateInfo stagingBufferAllocateInfo = {};
            stagingBufferAllocateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
            stagingBufferAllocateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
            stagingBufferAllocateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

            VkBuffer stagingBuffer = nullptr;
            VmaAllocation stagingBufferAllocation = nullptr;
            result = vmaCreateBuffer(allocatorHandle, &stagingBufferCreateInfo, &stagingBufferAllocateInfo,&stagingBuffer, &stagingBufferAllocation, nullptr);
            if (result != VK_SUCCESS)
                return false;

            result = vmaCopyMemoryToAllocation(allocatorHandle, createInfo.data, stagingBufferAllocation, 0, createInfo.dataSize);
            if (result != VK_SUCCESS)
                return false;


            CommandPool* commandPool = device->GetCommandPool();
            CommandBuffer commandBuffer = commandPool->AllocateCommandBuffer(CommandBufferLevel::Primary);

            if (commandBuffer.Begin({CommandBufferUsageFlagBits::OneTimeSubmit}))
            {
                VkImageMemoryBarrier toTransferBarrier { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
                toTransferBarrier.image = m_Image;
                toTransferBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                toTransferBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                toTransferBarrier.srcAccessMask = VK_ACCESS_NONE;
                toTransferBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                toTransferBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                toTransferBarrier.subresourceRange.baseMipLevel = 0;
                toTransferBarrier.subresourceRange.levelCount = createInfo.mips;
                toTransferBarrier.subresourceRange.baseArrayLayer = 0;
                toTransferBarrier.subresourceRange.layerCount = 1;
                toTransferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                toTransferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                vkCmdPipelineBarrier(commandBuffer.GetHandle(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
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
                imageCopyRegion.imageExtent = {createInfo.width, createInfo.height, 1};
                vkCmdCopyBufferToImage(commandBuffer.GetHandle(), stagingBuffer, m_Image,
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyRegion);

                VkImageMemoryBarrier toReadBarrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
                toReadBarrier.image = m_Image;
                toReadBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                toReadBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                toReadBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                toReadBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                toReadBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                toReadBarrier.subresourceRange.baseMipLevel = 0;
                toReadBarrier.subresourceRange.levelCount = createInfo.mips;
                toReadBarrier.subresourceRange.baseArrayLayer = 0;
                toReadBarrier.subresourceRange.layerCount = 1;
                toReadBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                toReadBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                vkCmdPipelineBarrier(commandBuffer.GetHandle(), VK_PIPELINE_STAGE_TRANSFER_BIT,VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &toReadBarrier);

                m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                commandBuffer.End();
            }
            else return false;

            Queue* graphicsQueue = device->GetGraphicsQueue();
            graphicsQueue->Submit(&commandBuffer, nullptr, nullptr, &fence);
            fence.Wait(~0);
            fence.Destroy();
            commandBuffer.Free();
            vmaDestroyBuffer(allocatorHandle, stagingBuffer, stagingBufferAllocation);
        }
        else
        {
            CommandPool* commandPool = device->GetCommandPool();
            CommandBuffer commandBuffer = commandPool->AllocateCommandBuffer(CommandBufferLevel::Primary);

            if (!commandBuffer.Begin({CommandBufferUsageFlagBits::OneTimeSubmit}))
                return false;

            VkImageMemoryBarrier toGeneralBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
            toGeneralBarrier.image = m_Image;
            toGeneralBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            toGeneralBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
            toGeneralBarrier.srcAccessMask = VK_ACCESS_NONE;
            toGeneralBarrier.dstAccessMask = VK_ACCESS_NONE;
            toGeneralBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            toGeneralBarrier.subresourceRange.baseMipLevel = 0;
            toGeneralBarrier.subresourceRange.levelCount = createInfo.mips;
            toGeneralBarrier.subresourceRange.baseArrayLayer = 0;
            toGeneralBarrier.subresourceRange.layerCount = 1;
            toGeneralBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            toGeneralBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            vkCmdPipelineBarrier(commandBuffer.GetHandle(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &toGeneralBarrier);

            commandBuffer.End();

            const Queue* graphicsQueue = device->GetGraphicsQueue();
            graphicsQueue->Submit(&commandBuffer, nullptr, nullptr, &fence);
            fence.Wait(~0);
            fence.Destroy();
            commandBuffer.Free();
            m_ImageLayout = VK_IMAGE_LAYOUT_GENERAL;
        }


        VkImageViewCreateInfo imageViewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        imageViewCreateInfo.image = m_Image;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = Convert<Format, VkFormat>(createInfo.format);
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = createInfo.mips;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(deviceHandle, &imageViewCreateInfo, nullptr, &m_ImageView) != VK_SUCCESS)
        {
            vmaDestroyImage(allocatorHandle, m_Image, m_Allocation);
            return false;
        }

        m_Device = device;
        m_Format = createInfo.format;
        m_Width = createInfo.width;
        m_Height = createInfo.height;
        m_Depth = createInfo.depth;
        m_Mips = createInfo.mips;
        m_SampleCount = createInfo.sampleCount;
        m_UsageFlags = createInfo.usageFlags;
        return true;
    }

    void Texture::Destroy()
    {
        const VkDevice deviceHandle = m_Device->GetHandle();
        const VmaAllocator allocatorHandle = m_Device->GetAllocator();
        vmaDestroyImage(allocatorHandle, m_Image, m_Allocation);
        vkDestroyImageView(deviceHandle, m_ImageView, nullptr);
        m_Device = nullptr;
    }

    bool Texture::IsValid()
    {
        return m_Device && m_Image && m_ImageView && m_Allocation;
    }

    VkImage Texture::GetImage() const
    {
        return m_Image;
    }

    VkImageView Texture::GetImageView() const
    {
        return m_ImageView;
    }

    VmaAllocation Texture::GetAllocation() const
    {
        return m_Allocation;
    }

    uint32_t Texture::GetImageLayout() const
    {
        return m_ImageLayout;
    }

    void Texture::SetImageLayout(const uint32_t imageLayout)
    {
        m_ImageLayout = imageLayout;
    }
}
