#include "VulkanVertexBuffer.h"
#include "VulkanCommandBuffer.h"
#include "Platform/PlatformRenderer.h"
#include "Runtime/Application.h"
#include "Runtime/Log.h"
#include <vk_mem_alloc.h>


namespace Nova
{
    VulkanVertexBuffer::VulkanVertexBuffer(Renderer* Renderer) : VertexBuffer(Renderer)
    {
    }



    VulkanVertexBuffer::VulkanVertexBuffer(Renderer* Renderer, const Vertex* Data, size_t Count) : VertexBuffer(Renderer, Data, Count)
    {
        VertexBuffer::SendData(Data, Count);

        const VulkanRenderer* CastedRenderer = static_cast<VulkanRenderer*>(Renderer);
        const VmaAllocator Allocator = CastedRenderer->GetAllocator();

        VkBufferCreateInfo BufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        BufferCreateInfo.size = Count * sizeof(Vertex);
        BufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
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
            NOVA_VULKAN_ERROR("Failed to create Vertex Buffer!");
            return;
        }

        if (VK_FAILED(vmaCopyMemoryToAllocation(Allocator, Data, m_Allocation, 0, Count * sizeof(Vertex))))
        {
            NOVA_VULKAN_ERROR("Failed to copy data into Vertex Buffer!");
            return;
        }
    }

    VulkanVertexBuffer::~VulkanVertexBuffer()
    {
        const VulkanRenderer* CastedRenderer = static_cast<VulkanRenderer*>(m_Renderer);
        const VkDevice Device = CastedRenderer->GetDevice();
        const VmaAllocator Allocator = CastedRenderer->GetAllocator();
        vkDeviceWaitIdle(Device);
        vmaDestroyBuffer(Allocator, m_Handle, m_Allocation);
    }

    void VulkanVertexBuffer::SendData(const Vertex* Data, size_t Count)
    {
        VertexBuffer::SendData(Data, Count);

        const VulkanRenderer* CastedRenderer = static_cast<VulkanRenderer*>(m_Renderer);
        const VmaAllocator Allocator = CastedRenderer->GetAllocator();

        if (m_Handle || m_Allocation)
        {
            const VkDevice Device = CastedRenderer->GetDevice();
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
        const VulkanRenderer* CastedRenderer = m_Renderer->As<VulkanRenderer>();
        const VkCommandBuffer Cmd = CastedRenderer->GetCurrentCommandBuffer()->GetHandle();
        constexpr VkDeviceSize Offsets[] = { 0 };
        vkCmdBindVertexBuffers(Cmd, 0, 1, &m_Handle, Offsets);
    }

    VkBuffer VulkanVertexBuffer::GetHandle() const
    {
        return m_Handle;
    }
}
