#include "VulkanIndexBuffer.h"
#include "Runtime/Log.h"
#include "VulkanCommandBuffer.h"
#include "VulkanRenderer.h"
#include "Runtime/Application.h"

namespace Nova
{
    VulkanIndexBuffer::VulkanIndexBuffer(Renderer* Owner) : IndexBuffer(Owner)
    {
    }


    VkBuffer VulkanIndexBuffer::GetHandle() const
    {
        return m_Handle;
    }

    bool VulkanIndexBuffer::Initialize(const IndexBufferCreateInfo& CreateInfo)
    {
        if (CreateInfo.Data == nullptr && CreateInfo.Size == 0)
            return true;

        const VulkanRenderer* Renderer = dynamic_cast<VulkanRenderer*>(m_Owner);
        const VmaAllocator Allocator = Renderer->GetAllocator();

        if (m_Handle || m_Allocation)
        {
            const VkDevice Device = Renderer->GetDevice();
            vkDeviceWaitIdle(Device);
            vmaDestroyBuffer(Allocator, m_Handle, m_Allocation);
        }

        VkBufferCreateInfo BufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        BufferCreateInfo.size = CreateInfo.Size;
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
            return false;
        }

        if (VK_FAILED(vmaCopyMemoryToAllocation(Allocator, CreateInfo.Data, m_Allocation, 0, CreateInfo.Size)))
        {
            return false;
        }
        return true;
    }

    void VulkanIndexBuffer::SetDebugName(const String& Name)
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

    void VulkanIndexBuffer::Destroy()
    {
        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        Renderer->WaitIdle();
        const VmaAllocator Allocator = Renderer->GetAllocator();
        vmaDestroyBuffer(Allocator, m_Handle, m_Allocation);
    }
}
