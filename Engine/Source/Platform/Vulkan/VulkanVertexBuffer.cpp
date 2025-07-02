#include "VulkanVertexBuffer.h"
#include "VulkanCommandBuffer.h"
#include "Platform/PlatformRenderer.h"
#include "Runtime/Application.h"
#include "Runtime/Log.h"
#include <vk_mem_alloc.h>


namespace Nova
{
    VulkanVertexBuffer::VulkanVertexBuffer(Renderer* Owner) : VertexBuffer(Owner)
    {
    }

    bool VulkanVertexBuffer::Initialize(const VertexBufferCreateInfo& CreateInfo)
    {
        if (CreateInfo.Data == nullptr && CreateInfo.Count == 0)
            return true;

        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const VmaAllocator Allocator = Renderer->GetAllocator();

        if (m_Handle || m_Allocation)
        {
            Renderer->WaitIdle();
            vmaDestroyBuffer(Allocator, m_Handle, m_Allocation);
        }

        VkBufferCreateInfo BufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        BufferCreateInfo.size = CreateInfo.Count * sizeof(Vertex);
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
            return false;
        }

        if (VK_FAILED(vmaCopyMemoryToAllocation(Allocator, CreateInfo.Data, m_Allocation, 0, CreateInfo.Count * sizeof(Vertex))))
        {
            return false;
        }
        return true;
    }

    void VulkanVertexBuffer::SetDebugName(const String& Name)
    {
        if constexpr(!RendererIsDebug)
            return;

        VkDebugUtilsObjectNameInfoEXT NameInfo { VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
        NameInfo.objectType = VK_OBJECT_TYPE_BUFFER;
        NameInfo.objectHandle = (u64)m_Handle;
        NameInfo.pObjectName = *Name;

        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        Renderer->GetFunctionPointers().vkSetDebugUtilsObjectNameEXT(Device, &NameInfo);
    }

    void VulkanVertexBuffer::Destroy()
    {
        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const VmaAllocator Allocator = Renderer->GetAllocator();
        Renderer->WaitIdle();
        vmaDestroyBuffer(Allocator, m_Handle, m_Allocation);
    }

    VkBuffer VulkanVertexBuffer::GetHandle() const
    {
        return m_Handle;
    }

    VkBuffer* VulkanVertexBuffer::GetHandlePtr()
    {
        return &m_Handle;
    }
}
