#include "VulkanTexture2D.h"
#include "VulkanRenderer.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "Runtime/Application.h"
#include "Runtime/Log.h"

namespace Nova
{
    VulkanTexture2D::VulkanTexture2D(const String& Name, u32 Width, u32 Height, const TextureParams& Params, u32 Slot) : Texture2D(Name, Width, Height, Params, Slot)
    {
        if (Width == 0 || Height == 0)
        {
            return;
        }
        const VulkanRenderer* Renderer = g_Application->GetRenderer()->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        const VmaAllocator Allocator = Renderer->GetAllocator();
        
        VkImageCreateInfo ImageCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        ImageCreateInfo.format = Renderer->Convertor.ConvertFormat(Params.Format);
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
        AllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        AllocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        
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
        ImageViewCreateInfo.format = Renderer->Convertor.ConvertFormat(Params.Format);
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

        const auto& Convertor = Renderer->Convertor;
        VkSamplerCreateInfo SamplerCreateInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        SamplerCreateInfo.magFilter = Convertor.ConvertFilter(Params.Filter);
        SamplerCreateInfo.minFilter = Convertor.ConvertFilter(Params.Filter);
        SamplerCreateInfo.addressModeU = Convertor.ConvertSamplerAddressMode(Params.AddressMode);
        SamplerCreateInfo.addressModeV = Convertor.ConvertSamplerAddressMode(Params.AddressMode);
        SamplerCreateInfo.addressModeW = Convertor.ConvertSamplerAddressMode(Params.AddressMode);
        SamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        SamplerCreateInfo.anisotropyEnable = true;
        SamplerCreateInfo.maxAnisotropy = 16.0f;
        SamplerCreateInfo.unnormalizedCoordinates = false;

        if (VK_FAILED(vkCreateSampler(Device, &SamplerCreateInfo, nullptr, &m_Sampler)))
        {
            NOVA_VULKAN_ERROR("Failed to create Vulkan Sampler !");
            return;
        }
    }

    VulkanTexture2D::~VulkanTexture2D()
    {
        const VulkanRenderer* Renderer = g_Application->GetRenderer()->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        const VmaAllocator Allocator = Renderer->GetAllocator();
        vmaDestroyImage(Allocator, m_Handle, m_Allocation);
        vkDestroyImageView(Device, m_ImageView, nullptr);
        vkDestroySampler(Device, m_Sampler, nullptr);
    }

    void VulkanTexture2D::SetTextureParameters(const TextureParams& Params)
    {
        m_Params = Params;
    }

    void VulkanTexture2D::SetData(u8* Data, u32 Width, u32 Height, const Format& Format)
    {
        const VulkanRenderer* Renderer = g_Application->GetRenderer()->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        const VmaAllocator Allocator = Renderer->GetAllocator();


        {
            m_Width = Width;
            m_Height = Height;
            m_Params.Format = Format;

            vkDestroySampler(Device, m_Sampler, nullptr);
            vmaDestroyImage(Allocator, m_Handle, m_Allocation);
            vkDestroyImageView(Device, m_ImageView, nullptr);


            VkImageCreateInfo ImageCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
            ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
            ImageCreateInfo.format = Renderer->Convertor.ConvertFormat(Format);
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
            AllocationCreateInfo.priority = 1.0f;
            AllocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;


            const VkResult Result = vmaCreateImage(Allocator,
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


            const VkDeviceSize Size =  (size_t)(Width * Height) * GetFormatSize(Format);

            VkBufferCreateInfo StagingBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
            StagingBufferInfo.size = Size;
            StagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            VmaAllocationCreateInfo StagingBufferAllocInfo = {};
            StagingBufferAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
            StagingBufferAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
            StagingBufferAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

            VkBuffer StagingBuffer = nullptr;
            VmaAllocation StagingBufferAllocation = nullptr;
            VmaAllocationInfo StagingBufferAllocInfoInfo { };
            vmaCreateBuffer(Allocator, &StagingBufferInfo, &StagingBufferAllocInfo, &StagingBuffer, &StagingBufferAllocation, &StagingBufferAllocInfoInfo);
            vmaCopyMemoryToAllocation(Allocator, Data, StagingBufferAllocation, 0, Size);


            VulkanCommandPool* CommandPool = Renderer->GetCommandPool();
            VulkanCommandBuffer* CommandBuffer = CommandPool->AllocateCommandBuffer({ CommandBufferLevel::Primary })->As<VulkanCommandBuffer>();

            VkFence Fence = nullptr;
            VkFenceCreateInfo FenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
            vkCreateFence(Device, &FenceCreateInfo, nullptr, &Fence);

            if (CommandBuffer->Begin({CommandBufferUsageFlagBits::OneTimeSubmit }))
            {
                VkImageMemoryBarrier Barrier1 { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
                Barrier1.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                Barrier1.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                Barrier1.srcAccessMask = 0;
                Barrier1.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                Barrier1.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                Barrier1.subresourceRange.baseMipLevel = 0;
                Barrier1.subresourceRange.levelCount = 1;
                Barrier1.subresourceRange.baseArrayLayer = 0;
                Barrier1.subresourceRange.layerCount = 1;
                Barrier1.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                Barrier1.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                Barrier1.image = m_Handle;
                vkCmdPipelineBarrier(CommandBuffer->GetHandle(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &Barrier1);

                VkBufferImageCopy Region { };
                Region.bufferOffset = 0;
                Region.bufferRowLength = 0;
                Region.bufferImageHeight = 0;
                Region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                Region.imageSubresource.mipLevel = 0;
                Region.imageSubresource.baseArrayLayer = 0;
                Region.imageSubresource.layerCount = 1;
                Region.imageOffset = { 0, 0, 0 };
                Region.imageExtent = { Width, Height, 1 };
                vkCmdCopyBufferToImage(CommandBuffer->GetHandle(), StagingBuffer, m_Handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &Region);

                VkImageMemoryBarrier Barrier2 = Barrier1;
                Barrier2.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                Barrier2.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                Barrier2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                Barrier2.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                vkCmdPipelineBarrier(CommandBuffer->GetHandle(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &Barrier2);
                CommandBuffer->End();

                VkCommandBuffer CommandBuffers[] = { CommandBuffer->GetHandle() };
                VkQueue GraphicsQueue = Renderer->GetGraphicsQueue();
                VkSubmitInfo SubmitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
                SubmitInfo.commandBufferCount = 1;
                SubmitInfo.pCommandBuffers = CommandBuffers;

                vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, Fence);
                vkWaitForFences(Device, 1, &Fence, true, U64_MAX);
                vkDestroyFence(Device, Fence, nullptr);
                CommandPool->FreeCommandBuffer(CommandBuffer);
            }

            vmaDestroyBuffer(Allocator, StagingBuffer, StagingBufferAllocation);






            VkImageViewCreateInfo ImageViewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
            ImageViewCreateInfo.image = m_Handle;
            ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            ImageViewCreateInfo.format = Renderer->Convertor.ConvertFormat(Format);
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
        }


        const auto& Convertor = Renderer->Convertor;
        VkSamplerCreateInfo SamplerCreateInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        SamplerCreateInfo.magFilter = Convertor.ConvertFilter(Filter::Linear);
        SamplerCreateInfo.minFilter = Convertor.ConvertFilter(Filter::Linear);
        SamplerCreateInfo.addressModeU = Convertor.ConvertSamplerAddressMode(SamplerAddressMode::Repeat);
        SamplerCreateInfo.addressModeV = Convertor.ConvertSamplerAddressMode(SamplerAddressMode::Repeat);
        SamplerCreateInfo.addressModeW = Convertor.ConvertSamplerAddressMode(SamplerAddressMode::Repeat);
        SamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        SamplerCreateInfo.anisotropyEnable = true;
        SamplerCreateInfo.maxAnisotropy = 16.0f;
        SamplerCreateInfo.unnormalizedCoordinates = false;

        if (VK_FAILED(vkCreateSampler(Device, &SamplerCreateInfo, nullptr, &m_Sampler)))
        {
            NOVA_VULKAN_ERROR("Failed to create Vulkan Sampler !");
            return;
        }
    }

    void VulkanTexture2D::Bind() const
    {
        
    }

    void VulkanTexture2D::Unbind() const
    {
    }

    uintptr_t VulkanTexture2D::GetHandle() const
    {
        return (uintptr_t)m_Handle;
    }

    VkImageView VulkanTexture2D::GetImageView() const
    {
        return m_ImageView;
    }

    VkSampler VulkanTexture2D::GetSampler() const
    {
        return m_Sampler;
    }
}
