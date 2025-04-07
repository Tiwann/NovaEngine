#include "VulkanIndexBuffer.h"
#include "Core/Application.h"

namespace Nova
{
    VulkanIndexBuffer::VulkanIndexBuffer()
    : m_Renderer(g_Application->GetRenderer<VulkanRenderer>())
    {

    }

    VulkanIndexBuffer::VulkanIndexBuffer(const u32* Indices, size_t Count)
    : IndexBuffer(Indices, Count), m_Renderer(g_Application->GetRenderer<VulkanRenderer>())
    {
        const VmaAllocator Allocator = m_Renderer->GetAllocator();

        VkBufferCreateInfo BufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        BufferCreateInfo.size = Count * sizeof(u32);
        BufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        BufferCreateInfo.flags = 0;
        BufferCreateInfo.pQueueFamilyIndices = nullptr;
        BufferCreateInfo.queueFamilyIndexCount = 0;

        VmaAllocationCreateInfo AllocCreateInfo = {};
        AllocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        AllocCreateInfo.priority = 1.0f;
        AllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        AllocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        if (VK_FAILED(vmaCreateBuffer(Allocator,
            &BufferCreateInfo,
            &AllocCreateInfo,
            &m_Handle,
            &m_Allocation,
            &m_AllocationInfo)))
        {
            NOVA_VULKAN_ERROR("Failed to create Index Buffer!");
            return;
        }

        if (VK_FAILED(vmaCopyMemoryToAllocation(Allocator, Indices, m_Allocation, 0, Count * sizeof(u32))))
        {
            NOVA_VULKAN_ERROR("Failed to copy data into Index Buffer!");
            return;
        }
    }

    VulkanIndexBuffer::~VulkanIndexBuffer()
    {
        const VkDevice Device = m_Renderer->GetDevice();
        const VmaAllocator Allocator = m_Renderer->GetAllocator();
        vkDeviceWaitIdle(Device);
        vmaDestroyBuffer(Allocator, m_Handle, m_Allocation);
    }

    void VulkanIndexBuffer::SendData(const u32* Indices, size_t Count)
    {
        IndexBuffer::SendData(Indices, Count);
        const VmaAllocator Allocator = m_Renderer->GetAllocator();

        if (m_Handle || m_Allocation)
        {
            const VkDevice Device = m_Renderer->GetDevice();
            vkDeviceWaitIdle(Device);
            vmaDestroyBuffer(Allocator, m_Handle, m_Allocation);
        }

        VkBufferCreateInfo BufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        BufferCreateInfo.size = Count * sizeof(u32);
        BufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        BufferCreateInfo.flags = 0;
        BufferCreateInfo.pQueueFamilyIndices = nullptr;
        BufferCreateInfo.queueFamilyIndexCount = 0;

        VmaAllocationCreateInfo AllocCreateInfo = {};
        AllocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        AllocCreateInfo.priority = 1.0f;
        AllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        AllocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        if (VK_FAILED(vmaCreateBuffer(Allocator,
            &BufferCreateInfo,
            &AllocCreateInfo,
            &m_Handle,
            &m_Allocation,
            &m_AllocationInfo)))
        {
            NOVA_VULKAN_ERROR("Failed to create Index Buffer!");
            return;
        }

        if (VK_FAILED(vmaCopyMemoryToAllocation(Allocator, Indices, m_Allocation, 0, Count * sizeof(u32))))
        {
            NOVA_VULKAN_ERROR("Failed to copy data into Index Buffer!");
            return;
        }
    }

    void VulkanIndexBuffer::Invalidate() const
    {
    }

    void VulkanIndexBuffer::Bind() const
    {
        const VkCommandBuffer Cmd = m_Renderer->GetCurrentCommandBuffer();
        vkCmdBindIndexBuffer(Cmd, m_Handle, 0, VK_INDEX_TYPE_UINT32);
    }
}
