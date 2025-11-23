#include "DescriptorPool.h"

#include <vulkan/vulkan.h>

#include "Conversions.h"
#include "Device.h"
#include "ShaderBindingSetLayout.h"

namespace Nova::Vulkan
{
    bool DescriptorPool::Initialize(const DescriptorPoolCreateInfo& createInfo)
    {
        Array<VkDescriptorPoolSize> poolSizes;
        for (const Pair<BindingType, uint32_t>& poolSize : createInfo.sizes)
        {
            VkDescriptorPoolSize descriptorPoolSize = {};
            descriptorPoolSize.type = Convert<BindingType, VkDescriptorType>(poolSize.key);
            descriptorPoolSize.descriptorCount = poolSize.value;
            poolSizes.Add(descriptorPoolSize);
        }

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        descriptorPoolCreateInfo.maxSets = createInfo.maxSets;
        descriptorPoolCreateInfo.pPoolSizes = poolSizes.Data();
        descriptorPoolCreateInfo.poolSizeCount = poolSizes.Count();

        Device* device = (Device*)createInfo.device;
        const VkDevice deviceHandle = device->GetHandle();
        const VkResult result = vkCreateDescriptorPool(deviceHandle, &descriptorPoolCreateInfo, nullptr, &m_Handle);
        if (result != VK_SUCCESS)
            return false;

        m_Device = device;

        return true;
    }

    void DescriptorPool::Destroy()
    {
        vkDestroyDescriptorPool(m_Device->GetHandle(), m_Handle, nullptr);
    }

    VkDescriptorPool DescriptorPool::GetHandle() const
    {
        return m_Handle;
    }

    const VkDescriptorPool* DescriptorPool::GetHandlePtr() const
    {
        return &m_Handle;
    }

    VkDescriptorSet DescriptorPool::AllocateDescriptorSet(const Nova::ShaderBindingSetLayout& bindingSetLayout) const
    {
        VkDescriptorSet descriptorSet = nullptr;
        const VkDescriptorSetLayout descriptorSetLayout = ((const ShaderBindingSetLayout&)bindingSetLayout).GetHandle();

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        descriptorSetAllocateInfo.descriptorPool = m_Handle;
        descriptorSetAllocateInfo.descriptorSetCount = 1;
        descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;
        if (vkAllocateDescriptorSets(m_Device->GetHandle(), &descriptorSetAllocateInfo, &descriptorSet) != VK_SUCCESS)
            return nullptr;
        return descriptorSet;
    }
}
