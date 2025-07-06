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
        VulkanDescriptorPool* Owner = m_Pool->As<VulkanDescriptorPool>();
        const VulkanRenderer* Renderer = Owner->GetOwner()->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        DescriptorSetAllocateInfo.descriptorPool = Owner->GetHandle();
        DescriptorSetAllocateInfo.descriptorSetCount = 1;
        DescriptorSetAllocateInfo.
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
