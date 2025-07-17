#include "Buffer.h"
#include "Device.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>


namespace Nova::Vulkan
{
    static VkBufferUsageFlags GetBufferUsage(const Rendering::BufferUsage& usage)
    {
        switch (usage)
        {
        case Rendering::BufferUsage::None: return 0;
        case Rendering::BufferUsage::VertexBuffer: return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        case Rendering::BufferUsage::IndexBuffer: return VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        case Rendering::BufferUsage::UniformBuffer: return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        case Rendering::BufferUsage::StorageBuffer: return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        case Rendering::BufferUsage::StagingBuffer: return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        default: return 0;
        }
    }

    Buffer::Buffer(Buffer&& other) noexcept
    {
        if (this == &other)
            return;

        m_Handle = other.m_Handle;
        m_Allocation = other.m_Allocation;
        m_Device = other.m_Device;
        m_Size = other.m_Size;
        m_Usage = other.m_Usage;

        other.m_Handle = nullptr;
        other.m_Allocation = nullptr;
        other.m_Device = nullptr;
        other.m_Size = 0;
        other.m_Usage = Rendering::BufferUsage::None;
    }

    Buffer& Buffer::operator=(Buffer&& other) noexcept
    {
        if (this == &other)
            return *this;

        m_Handle = other.m_Handle;
        m_Allocation = other.m_Allocation;
        m_Device = other.m_Device;
        m_Size = other.m_Size;
        m_Usage = other.m_Usage;

        other.m_Handle = nullptr;
        other.m_Allocation = nullptr;
        other.m_Device = nullptr;
        other.m_Size = 0;
        other.m_Usage = Rendering::BufferUsage::None;
        return *this;
    }

    bool Buffer::Initialize(const Rendering::BufferCreateInfo& createInfo)
    {
        VkBufferCreateInfo bufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferCreateInfo.size = createInfo.size;
        bufferCreateInfo.usage = GetBufferUsage(createInfo.usage);

        VmaAllocationCreateInfo bufferAllocationCreateInfo = {};

        if (createInfo.usage == Rendering::BufferUsage::StagingBuffer)
        {
            bufferAllocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
            bufferAllocationCreateInfo.priority = 0.5f;
            bufferAllocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
            bufferAllocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        }

        if (createInfo.usage == Rendering::BufferUsage::VertexBuffer || createInfo.usage == Rendering::BufferUsage::IndexBuffer)
        {
            bufferAllocationCreateInfo.priority = 1.0f;
            bufferAllocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
            bufferAllocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        }


        Device* device = static_cast<Device*>(createInfo.device);
        const VmaAllocator allocatorHandle = device->GetAllocator();
        vmaDestroyBuffer(allocatorHandle, m_Handle, m_Allocation);
        const VkResult result = vmaCreateBuffer(allocatorHandle, &bufferCreateInfo, &bufferAllocationCreateInfo, &m_Handle, &m_Allocation, nullptr);
        if (result != VK_SUCCESS)
            return false;

        m_Device = device;
        m_Size = createInfo.size;
        m_Usage = createInfo.usage;
        return true;
    }

    void Buffer::Destroy()
    {
        m_Device->WaitIdle();
        const VmaAllocator allocatorHandle = m_Device->GetAllocator();
        vmaDestroyBuffer(allocatorHandle, m_Handle, m_Allocation);
    }

    bool Buffer::Resize(size_t newSize, bool keepData)
    {
        return false;
    }

    bool Buffer::CopyData(const void* src, const size_t offset, const size_t size)
    {
        const VmaAllocator allocatorHandle = m_Device->GetAllocator();
        const VkResult result = vmaCopyMemoryToAllocation(allocatorHandle, src, m_Allocation, offset, size);
        if (result != VK_SUCCESS)
            return false;
        return true;
    }

    bool Buffer::CopyTo(const Rendering::Buffer& other, const size_t srcOffset, const size_t destOffset, const size_t size)
    {
        Rendering::CommandBufferAllocateInfo allocateInfo;
        allocateInfo.commandPool = m_Device->GetCommandPool();;
        allocateInfo.device = m_Device;
        allocateInfo.level = Rendering::CommandBufferLevel::Primary;

        CommandBuffer commandBuffer;
        if (!commandBuffer.Allocate(allocateInfo))
            return false;

        Rendering::CommandBufferBeginInfo beginInfo;
        beginInfo.Flags = Rendering::CommandBufferUsageFlagBits::OneTimeSubmit;

        if (commandBuffer.Begin(beginInfo))
        {
            const VkCommandBuffer cmdBuff = commandBuffer.GetHandle();
            const VkBuffer src = m_Handle;
            const VkBuffer dst = static_cast<const Buffer&>(other).m_Handle;

            VkBufferCopy2 region = { VK_STRUCTURE_TYPE_BUFFER_COPY_2 };
            region.srcOffset = srcOffset;
            region.dstOffset = destOffset;
            region.size = size;

            VkCopyBufferInfo2 copyInfo { VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2 };
            copyInfo.srcBuffer = src;
            copyInfo.dstBuffer = dst;
            copyInfo.regionCount = 1;
            copyInfo.pRegions = &region;
            vkCmdCopyBuffer2(cmdBuff, &copyInfo);
            commandBuffer.End();

            Fence fence;
            fence.Initialize(Rendering::FenceCreateInfo(m_Device));

            const Queue* graphicsQueue = m_Device->GetGraphicsQueue();
            graphicsQueue->Submit(&commandBuffer, nullptr, nullptr, &fence);
            fence.Wait();
            commandBuffer.Free();
            fence.Destroy();
        } else return false;

        return true;
    }

    VkBuffer Buffer::GetHandle() const
    {
        return m_Handle;
    }

    const VkBuffer* Buffer::GetHandlePtr() const
    {
        return &m_Handle;
    }
}
