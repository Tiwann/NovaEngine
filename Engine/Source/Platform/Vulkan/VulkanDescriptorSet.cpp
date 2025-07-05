#include "VulkanDescriptorSet.h"
#include <vulkan/vulkan.h>

#include "VulkanDescriptorPool.h"
#include "VulkanRenderer.h"
#include "Rendering/Renderer.h"

namespace Nova
{
    VulkanDescriptorSet::VulkanDescriptorSet(DescriptorPool* Owner) : DescriptorSet(Owner)
    {

    }

    bool VulkanDescriptorSet::Initialize(const DescriptorSetAllocateInfo& AllocateInfo)
    {
        const VkDevice Device = m_Owner->As<VulkanRenderer>()->GetDevice();
        VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        DescriptorSetAllocateInfo.descriptorPool = m_Owner->As<VulkanDescriptorPool>()->
        vkAllocateDescriptorSets(Device, )
    }

    void VulkanDescriptorSet::Destroy()
    {
    }

    void VulkanDescriptorSet::SetDebugName(const String& Name)
    {
        DescriptorSet::SetDebugName(Name);
    }

    VkDescriptorSet VulkanDescriptorSet::GetHandle() const
    {
        return m_Handle;
    }

    const VkDescriptorSet* VulkanDescriptorSet::GetHandlePtr() const
    {
        return &m_Handle;
    }
}
