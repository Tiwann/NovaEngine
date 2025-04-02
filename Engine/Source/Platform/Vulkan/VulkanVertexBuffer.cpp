#include "VulkanVertexBuffer.h"
#include "Platform/PlatformRenderer.h"
#include "Core/Application.h"
#include <vk_mem_alloc.h>

namespace Hydro
{
    VulkanVertexBuffer::VulkanVertexBuffer() : m_Renderer(g_Application->GetRenderer<VulkanRenderer>())
    {
    }

    VulkanVertexBuffer::~VulkanVertexBuffer()
    {
        const VkDevice Device = m_Renderer->GetDevice();
        const VmaAllocator Allocator = m_Renderer->GetAllocator();
        vkDeviceWaitIdle(Device);
        vmaDestroyBuffer(Allocator, m_Handle, m_Allocation);
    }

    VulkanVertexBuffer::VulkanVertexBuffer(const Vertex* Data, size_t Count) : VertexBuffer(Data, Count),
    m_Renderer(g_Application->GetRenderer<VulkanRenderer>())
    {
        VertexBuffer::SendData(Data, Count);
        
        const VmaAllocator Allocator = m_Renderer->GetAllocator();

        VkBufferCreateInfo BufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        BufferCreateInfo.size = Count * sizeof(VertexBuffer);
        BufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
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
            NOVA_VULKAN_ERROR("Failed to create Vertex Buffer!");
            return;
        }

        if (VK_FAILED(vmaCopyMemoryToAllocation(Allocator, Data, m_Allocation, 0, Count * sizeof(Vertex))))
        {
            NOVA_VULKAN_ERROR("Failed to copy data into Vertex Buffer!");
            return;
        }
    }

    void VulkanVertexBuffer::SendData(const Vertex* Data, size_t Count)
    {
        VertexBuffer::SendData(Data, Count);

        const VmaAllocator Allocator = m_Renderer->GetAllocator();

        if (m_Handle || m_Allocation)
        {
            const VkDevice Device = m_Renderer->GetDevice();
            vkDeviceWaitIdle(Device);
            vmaDestroyBuffer(Allocator, m_Handle, m_Allocation);
        }

        VkBufferCreateInfo BufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        BufferCreateInfo.size = Count * sizeof(u32);
        BufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
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
            NOVA_VULKAN_ERROR("Failed to create Vertex Buffer!");
            return;
        }

        if (VK_FAILED(vmaCopyMemoryToAllocation(Allocator, Data, m_Allocation, 0, Count * sizeof(Vertex))))
        {
            NOVA_VULKAN_ERROR("Failed to copy data into Index Buffer!");
            return;
        }
    }

    void VulkanVertexBuffer::Bind() const
    {
        const VkCommandBuffer Cmd = m_Renderer->GetCurrentCommandBuffer();
        constexpr VkDeviceSize Offsets[] = { 0 };
        vkCmdBindVertexBuffers(Cmd, 0, 1, &m_Handle, Offsets);
    }
}
