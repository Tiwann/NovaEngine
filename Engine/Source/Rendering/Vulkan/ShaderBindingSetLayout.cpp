#include "ShaderBindingSetLayout.h"
#include "RenderDevice.h"
#include "Conversions.h"
#include <vulkan/vulkan.h>

namespace Nova::Vulkan
{
    static constexpr uint32_t MAX_TEXTURES = 1024;
    bool ShaderBindingSetLayout::Build()
    {
        Array<VkDescriptorSetLayoutBinding> bindings;
        Array<VkDescriptorBindingFlags> bindingFlags;

        for (size_t i = 0; i < m_Bindings.Count(); i++)
        {
            const ShaderBinding& binding = m_Bindings[i];
            VkDescriptorSetLayoutBinding vkBinding = { };
            vkBinding.binding = i;
            if (binding.bindingType == BindingType::SampledTexture && binding.arrayCount == 0)
                vkBinding.descriptorCount = MAX_TEXTURES;
            else
                vkBinding.descriptorCount = binding.arrayCount;
            vkBinding.stageFlags = Convert<VkShaderStageFlags>(binding.stageFlags);
            vkBinding.descriptorType = Convert<VkDescriptorType>(binding.bindingType);
            vkBinding.pImmutableSamplers = nullptr;

            uint32_t bindingFlag = binding.arrayCount > 1 && binding.bindingType == BindingType::SampledTexture
                                       ? VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT |
                                       VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
                                       VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT
                                       : 0;
            bindingFlags.Add(bindingFlag);
            bindings.Add(vkBinding);
        }

        VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagCreateInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO};
        bindingFlagCreateInfo.bindingCount = m_Bindings.Count();
        bindingFlagCreateInfo.pBindingFlags = bindingFlags.Data();

        VkDescriptorSetLayoutCreateInfo layoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        layoutCreateInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
        layoutCreateInfo.pNext = &bindingFlagCreateInfo;
        layoutCreateInfo.bindingCount = bindings.Count();
        layoutCreateInfo.pBindings = bindings.Data();

        const VkDevice deviceHandle = m_Device->GetHandle();

        vkDestroyDescriptorSetLayout(deviceHandle, m_Handle, nullptr);
        if (vkCreateDescriptorSetLayout(deviceHandle, &layoutCreateInfo, nullptr, &m_Handle) != VK_SUCCESS)
            return false;

        return true;
    }

    bool ShaderBindingSetLayout::Initialize(Nova::RenderDevice* device, uint32_t setIndex)
    {
        if (!device) return false;
        m_Device = (RenderDevice*)device;
        m_SetIndex = setIndex;
        return true;
    }

    void ShaderBindingSetLayout::Destroy()
    {
        const VkDevice deviceHandle = m_Device->GetHandle();
        vkDestroyDescriptorSetLayout(deviceHandle, m_Handle, nullptr);
    }

    const VkDescriptorSetLayout& ShaderBindingSetLayout::GetHandle() const
    {
        return m_Handle;
    }
}
