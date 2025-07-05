#include "VulkanDescriptorPool.h"
#include "VulkanRenderer.h"
#include "Runtime/Log.h"
#include <vulkan/vulkan.h>

#include "Rendering/DescriptorSet.h"

namespace Nova
{
    bool VulkanDescriptorPool::Initialize(const DescriptorPoolCreateInfo& CreateInfo)
    {
        Array<VkDescriptorPoolSize> PoolSizes;
        for (const Map<DescriptorType, u32>::PairType& PoolSize : CreateInfo.PoolSizes)
        {
            VkDescriptorPoolSize VkPoolSize;
            VkPoolSize.descriptorCount = PoolSize.Value;
        }

        VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        DescriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        DescriptorPoolCreateInfo.maxSets = CreateInfo.MaxDescriptorSets;
        DescriptorPoolCreateInfo.poolSizeCount = PoolSizes.Count();
        DescriptorPoolCreateInfo.pPoolSizes = PoolSizes.Data();

        const VkDevice Device = m_Owner->As<VulkanRenderer>()->GetDevice();

        if (VK_FAILED(vkCreateDescriptorPool(Device, &DescriptorPoolCreateInfo, nullptr, &m_Handle)))
        {
            NOVA_VULKAN_ERROR("Failed to create descriptor pool!");
            return false;
        }
    }

    void VulkanDescriptorPool::SetDebugName(const String& Name)
    {
        if constexpr (!RendererIsDebug)
            return;

        VkDebugUtilsObjectNameInfoEXT NameInfo{VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT};
        NameInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_POOL;
        NameInfo.objectHandle = (u64)m_Handle;
        NameInfo.pObjectName = *Name;

        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        Renderer->GetFunctionPointers().vkSetDebugUtilsObjectNameEXT(Device, &NameInfo);
    }

    void VulkanDescriptorPool::Destroy()
    {
        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        vkDestroyDescriptorPool(Device, m_Handle, nullptr);
    }

    VkDescriptorPool VulkanDescriptorPool::GetHandle() const
    {
        return m_Handle;
    }

    const VkDescriptorPool* VulkanDescriptorPool::GetHandlePtr() const
    {
        return &m_Handle;
    }

    DescriptorSet* VulkanDescriptorPool::AllocateDescriptorSet(const DescriptorSetAllocateInfo& AllocateInfo)
    {

    }
}
