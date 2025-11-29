#include "ShaderBindingSet.h"
#include "DescriptorPool.h"
#include "Texture.h"
#include "Device.h"
#include "Buffer.h"
#include "Sampler.h"
#include "ShaderBindingSetLayout.h"
#include <vulkan/vulkan.h>


namespace Nova::Vulkan
{
    bool ShaderBindingSet::Initialize(const ShaderBindingSetCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        if (!createInfo.layout) return false;
        if (!createInfo.pool) return false;

        const DescriptorPool* descriptorPool = (DescriptorPool*)createInfo.pool;
        const VkDescriptorSetLayout descriptorSetLayout = ((ShaderBindingSetLayout*)createInfo.layout)->GetHandle();

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        descriptorSetAllocateInfo.descriptorPool = descriptorPool->GetHandle();
        descriptorSetAllocateInfo.descriptorSetCount = 1;
        descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

        if (vkAllocateDescriptorSets(((Device*)createInfo.device)->GetHandle(), &descriptorSetAllocateInfo, &m_Handle) != VK_SUCCESS)
            return false;

        m_Device = (Device*)createInfo.device;
        m_DescriptorPool = (DescriptorPool*)createInfo.pool;
        m_BindingSetLayout = (ShaderBindingSetLayout*)createInfo.layout;
        return true;
    }

    void ShaderBindingSet::Destroy()
    {
        vkFreeDescriptorSets(m_Device->GetHandle(), m_DescriptorPool->GetHandle(), 1, &m_Handle);
    }

    VkDescriptorSet ShaderBindingSet::GetHandle() const
    {
        return m_Handle;
    }

    const VkDescriptorSet* ShaderBindingSet::GetHandlePtr() const
    {
        return &m_Handle;
    }

    bool ShaderBindingSet::BindTexture(const uint32_t binding, const Ref<Nova::Texture>& texture)
    {
        if (!texture) return false;

        VkDescriptorImageInfo imageInfo;
        imageInfo.imageLayout = (VkImageLayout)texture.As<Texture>()->GetImageLayout();
        imageInfo.imageView = texture.As<Texture>()->GetImageView();

        VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        if (texture->GetUsageFlags().Contains(TextureUsageFlagBits::Storage))
            descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

        VkWriteDescriptorSet write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write.descriptorType = descriptorType;
        write.descriptorCount = 1;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.pImageInfo = &imageInfo;
        write.dstSet = m_Handle;
        vkUpdateDescriptorSets(m_Device->GetHandle(), 1, &write, 0, nullptr);
        return true;
    }

    bool ShaderBindingSet::BindSampler(const uint32_t binding, const Ref<Nova::Sampler>& sampler)
    {
        if (!sampler) return false;

        VkDescriptorImageInfo imageInfo;
        imageInfo.sampler = sampler.As<Sampler>()->GetHandle();

        VkWriteDescriptorSet write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        write.descriptorCount = 1;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.pImageInfo = &imageInfo;
        write.dstSet = m_Handle;
        vkUpdateDescriptorSets(m_Device->GetHandle(), 1, &write, 0, nullptr);
        return true;
    }

    bool ShaderBindingSet::BindCombinedSamplerTexture(const uint32_t binding, const Ref<Nova::Sampler>& sampler, const Ref<Nova::Texture>& texture)
    {
        if (!sampler) return false;
        if (!texture) return false;

        VkDescriptorImageInfo imageInfo;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = texture.As<Texture>()->GetImageView();
        imageInfo.sampler = sampler.As<Sampler>()->GetHandle();

        VkWriteDescriptorSet write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = 1;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.pImageInfo = &imageInfo;
        write.dstSet = m_Handle;
        vkUpdateDescriptorSets(m_Device->GetHandle(), 1, &write, 0, nullptr);
        return true;
    }

    bool ShaderBindingSet::BindBuffer(const uint32_t binding, const Ref<Nova::Buffer>& buffer, const size_t offset, const size_t size)
    {
        if (!buffer) return false;

        VkDescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = buffer.As<Buffer>()->GetHandle();
        bufferInfo.offset = offset;
        bufferInfo.range = size;

        VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
        switch (buffer->GetUsage())
        {
        case BufferUsage::None:
        case BufferUsage::VertexBuffer:
        case BufferUsage::IndexBuffer:
        case BufferUsage::UniformBuffer:
            descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            break;
        case BufferUsage::StorageBuffer:
            descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            break;
        case BufferUsage::StagingBuffer:
            break;
        }

        if (descriptorType == VK_DESCRIPTOR_TYPE_MAX_ENUM)
            return false;


        VkWriteDescriptorSet write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write.descriptorType = descriptorType;
        write.descriptorCount = 1;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.pBufferInfo = &bufferInfo;
        write.dstSet = m_Handle;
        vkUpdateDescriptorSets(m_Device->GetHandle(), 1, &write, 0, nullptr);
        return true;
    }
}
