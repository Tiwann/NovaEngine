#include "Sampler.h"
#include "Device.h"
#include "Conversions.h"
#include <vulkan/vulkan.h>


namespace Nova::Vulkan
{
    bool Sampler::Initialize(const SamplerCreateInfo& createInfo)
    {
        VkSamplerCreateInfo samplerCreateInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        samplerCreateInfo.addressModeU = Convert<VkSamplerAddressMode>(createInfo.addressModeU);
        samplerCreateInfo.addressModeV = Convert<VkSamplerAddressMode>(createInfo.addressModeV);
        samplerCreateInfo.addressModeW = Convert<VkSamplerAddressMode>(createInfo.addressModeW);
        samplerCreateInfo.anisotropyEnable = createInfo.anisotropyEnable;
        samplerCreateInfo.compareEnable = createInfo.compareEnable;
        samplerCreateInfo.compareOp = Convert<VkCompareOp>(createInfo.compareOp);
        samplerCreateInfo.minFilter = Convert<VkFilter>(createInfo.minFilter);
        samplerCreateInfo.magFilter = Convert<VkFilter>(createInfo.magFilter);
        samplerCreateInfo.minLod = createInfo.minLod;
        samplerCreateInfo.maxLod = createInfo.maxLod;
        samplerCreateInfo.unnormalizedCoordinates = createInfo.unnormalizedCoordinates;
        samplerCreateInfo.mipLodBias = 0.0f;
        samplerCreateInfo.mipmapMode = Convert<VkSamplerMipmapMode>(createInfo.mipmapFilter);

        const VkDevice deviceHandle = ((Device*)createInfo.device)->GetHandle();
        const VkResult result = vkCreateSampler(deviceHandle, &samplerCreateInfo,nullptr, &m_Handle);
        if (result != VK_SUCCESS)
            return false;

        m_Device = (Device*)createInfo.device;
        m_AddressModeU = createInfo.addressModeU;
        m_AddressModeV = createInfo.addressModeV;
        m_AddressModeW = createInfo.addressModeW;
        m_MinFilter = createInfo.minFilter;
        m_MagFilter = createInfo.magFilter;
        m_AnisotropyEnable = createInfo.anisotropyEnable;
        m_CompareEnable = createInfo.compareEnable;
        m_CompareOp = createInfo.compareOp;
        m_UnnormalizedCoordinates = createInfo.unnormalizedCoordinates;
        m_MinLod = createInfo.minLod;
        m_MaxLod = createInfo.maxLod;
        m_MipmapFilter = createInfo.mipmapFilter;
        return true;
    }

    void Sampler::Destroy()
    {
        const VkDevice deviceHandle = m_Device->GetHandle();
        vkDestroySampler(deviceHandle, m_Handle, nullptr);
    }

    VkSampler Sampler::GetHandle() const
    {
        return m_Handle;
    }
}
