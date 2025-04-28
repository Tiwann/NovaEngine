#include "VulkanIndexBuffer.h"

#include "VulkanCommandBuffer.h"
#include "VulkanRenderer.h"
#include "Runtime/Application.h"

namespace Nova
{
    VulkanIndexBuffer::VulkanIndexBuffer(Renderer* Renderer) : IndexBuffer(Renderer)
    {

    }

    VulkanIndexBuffer::VulkanIndexBuffer(Renderer* Renderer, const u32* Indices, size_t Count)
    : IndexBuffer(Renderer, Indices, Count)
    {
        const VulkanRenderer* CastedRenderer = dynamic_cast<VulkanRenderer*>(m_Renderer);
        const VmaAllocator Allocator = CastedRenderer->GetAllocator();

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
        AllocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

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
        const VulkanRenderer* CastedRenderer = dynamic_cast<VulkanRenderer*>(m_Renderer);
        const VkDevice Device = CastedRenderer->GetDevice();
        const VmaAllocator Allocator = CastedRenderer->GetAllocator();
        vkDeviceWaitIdle(Device);
        vmaDestroyBuffer(Allocator, m_Handle, m_Allocation);
    }

    void VulkanIndexBuffer::SendData(const u32* Indices, size_t Count)
    {
        IndexBuffer::SendData(Indices, Count);
        const VulkanRenderer* CastedRenderer = dynamic_cast<VulkanRenderer*>(m_Renderer);
        const VmaAllocator Allocator = CastedRenderer->GetAllocator();

        if (m_Handle || m_Allocation)
        {
            const VkDevice Device = CastedRenderer->GetDevice();
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
        const VulkanRenderer* CastedRenderer = dynamic_cast<VulkanRenderer*>(m_Renderer);
        const VkCommandBuffer Cmd = CastedRenderer->GetCurrentCommandBuffer()->GetHandle();
        vkCmdBindIndexBuffer(Cmd, m_Handle, 0, VK_INDEX_TYPE_UINT32);
    }
}
