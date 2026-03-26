#include "Buffer.h"
#include "RenderDevice.h"
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>


namespace Nova::Vulkan
{
    static VkBufferUsageFlags GetBufferUsage(const BufferUsage& usage)
    {
        switch (usage)
        {
        case BufferUsage::None: return 0;
        case BufferUsage::VertexBuffer: return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        case BufferUsage::IndexBuffer: return VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        case BufferUsage::UniformBuffer: return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        case BufferUsage::StorageBuffer: return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        case BufferUsage::StagingBuffer: return VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        case BufferUsage::IndirectBuffer: return VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        default: return 0;
        }
    }

    bool Buffer::Initialize(const BufferCreateInfo& createInfo)
    {
        VkBufferCreateInfo bufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferCreateInfo.size = createInfo.size;
        bufferCreateInfo.usage = GetBufferUsage(createInfo.usage);

        VmaAllocationCreateInfo bufferAllocationCreateInfo = {};

        switch (createInfo.usage)
        {
        case BufferUsage::None:
            return false;
        case BufferUsage::VertexBuffer:
        case BufferUsage::IndexBuffer:
        case BufferUsage::UniformBuffer:
        case BufferUsage::StorageBuffer:
        case BufferUsage::IndirectBuffer:
            {
                bufferAllocationCreateInfo.priority = 1.0f;
                bufferAllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
                if (createInfo.mapped)
                {
                    bufferAllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
                    bufferAllocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT
                        | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
                }

            }
            break;
        case BufferUsage::StagingBuffer:
            {
                bufferAllocationCreateInfo.priority = 0.5f;
                bufferAllocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
                bufferAllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
                if (createInfo.mapped)
                {
                    bufferAllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
                    bufferAllocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
                }
            }
            break;
        }

        RenderDevice* device = static_cast<RenderDevice*>(createInfo.device);
        const VmaAllocator allocatorHandle = device->GetAllocator();
        vmaDestroyBuffer(allocatorHandle, m_Handle, m_Allocation);
        VmaAllocationInfo allocationInfo;
        const VkResult result = vmaCreateBuffer(allocatorHandle, &bufferCreateInfo, &bufferAllocationCreateInfo, &m_Handle, &m_Allocation, &allocationInfo);
        if (result != VK_SUCCESS)
            return false;

        m_Device = device;
        m_Size = createInfo.size;
        m_Usage = createInfo.usage;
        m_Mapped = createInfo.mapped;
        m_MappedData = m_Mapped ? allocationInfo.pMappedData : nullptr;
        return true;
    }

    void Buffer::Destroy()
    {
        m_Device->WaitIdle();
        const VmaAllocator allocatorHandle = m_Device->GetAllocator();
        vmaDestroyBuffer(allocatorHandle, m_Handle, m_Allocation);
    }

    void* Buffer::Map()
    {
        if (m_Mapped) return m_MappedData;
        const VmaAllocator allocatorHandle = m_Device->GetAllocator();
        void* mappedMemory = nullptr;
        vmaMapMemory(allocatorHandle, m_Allocation, &mappedMemory);
        return mappedMemory;
    }

    void Buffer::Unmap(const void* ptr)
    {
        if (m_Mapped) return;
        const VmaAllocator allocatorHandle = m_Device->GetAllocator();
        vmaUnmapMemory(allocatorHandle, m_Allocation);
    }

    VkBuffer Buffer::GetHandle() const
    {
        return m_Handle;
    }

    const VkBuffer* Buffer::GetHandlePtr() const
    {
        return &m_Handle;
    }

    void Buffer::GetAllocationInfo(VmaAllocationInfo2& outAllocationInfo) const
    {
        const VmaAllocator allocatorHandle = m_Device->GetAllocator();
        vmaGetAllocationInfo2(allocatorHandle, m_Allocation, &outAllocationInfo);
    }
}
