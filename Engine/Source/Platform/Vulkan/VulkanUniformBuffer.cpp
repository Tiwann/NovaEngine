#include "VulkanUniformBuffer.h"
#include "Runtime/Log.h"
#include "VulkanRenderer.h"
#include "Rendering/Renderer.h"

namespace Nova
{
    VulkanUniformBuffer::VulkanUniformBuffer(Renderer* Owner) : UniformBuffer(Owner)
    {
    }

    bool VulkanUniformBuffer::Allocate(size_t Size)
    {
        const VmaAllocator Allocator = m_Owner->As<VulkanRenderer>()->GetAllocator();

        VkBufferCreateInfo BufferCreateInfo { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        BufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        BufferCreateInfo.size = Size;
        BufferCreateInfo.pQueueFamilyIndices = nullptr;
        BufferCreateInfo.queueFamilyIndexCount = 0;

        VmaAllocationCreateInfo AllocCreateInfo = {};
        AllocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        AllocCreateInfo.priority = 1.0f;
        AllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        AllocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;


        if (VK_FAILED(vmaCreateBuffer(Allocator,
            &BufferCreateInfo,
            &AllocCreateInfo,
            &m_Handle,
            &m_Allocation,
            &m_AllocationInfo)))
        {
            NOVA_VULKAN_ERROR("Failed to create uniform Buffer!");
            return false;
        }

        return true;
    }

    void VulkanUniformBuffer::Free()
    {
        const VmaAllocator Allocator = m_Owner->As<VulkanRenderer>()->GetAllocator();
        vmaDestroyBuffer(Allocator, m_Handle, m_Allocation);
    }

    void VulkanUniformBuffer::Copy(const void* Src, const size_t Size, const size_t Offset)
    {
        const VmaAllocator Allocator = m_Owner->As<VulkanRenderer>()->GetAllocator();
        vmaCopyMemoryToAllocation(Allocator, Src, m_Allocation, Offset, Size);
    }
}
