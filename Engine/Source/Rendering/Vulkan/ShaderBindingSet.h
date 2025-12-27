#pragma once
#include "Rendering/ShaderBindingSet.h"

typedef struct VkDescriptorSet_T* VkDescriptorSet;

namespace Nova::Vulkan
{
    class Device;
    class DescriptorPool;

    class ShaderBindingSet final : public Nova::ShaderBindingSet
    {
    public:
        bool Initialize(const ShaderBindingSetCreateInfo& createInfo) override;
        void Destroy() override;

        VkDescriptorSet GetHandle() const;
        const VkDescriptorSet* GetHandlePtr() const;
        bool BindTextures(uint32_t binding, const Nova::Texture* const* textures, size_t textureCount, BindingType bindingType) override;
        bool BindTexture(uint32_t binding, const Nova::Texture& texture, BindingType bindingType) override;
        bool BindSampler(uint32_t binding, const Nova::Sampler& sampler) override;
        bool BindCombinedSamplerTexture(uint32_t binding, const Nova::Sampler& sampler, const Nova::Texture& texture) override;
        bool BindBuffer(uint32_t binding, const Nova::Buffer& buffer, size_t offset, size_t size) override;
    private:
        Device* m_Device = nullptr;
        DescriptorPool* m_DescriptorPool = nullptr;
        VkDescriptorSet m_Handle = nullptr;
    };
}
