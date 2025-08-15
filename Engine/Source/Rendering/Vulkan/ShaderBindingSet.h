#pragma once
#include "Rendering/ShaderBindingSet.h"

typedef struct VkDescriptorSet_T* VkDescriptorSet;

namespace Nova::Vulkan
{
    class Device;
    class DescriptorPool;

    class ShaderBindingSet : public Rendering::ShaderBindingSet
    {
    public:
        bool Initialize(const Rendering::ShaderBindingSetCreateInfo& createInfo) override;
        void Destroy() override;

        VkDescriptorSet GetHandle() const;
        const VkDescriptorSet* GetHandlePtr() const;
        bool BindTexture(uint32_t binding, const Ref<Rendering::Texture>& texture) override;
        bool BindSampler(uint32_t binding, const Ref<Rendering::Sampler>& sampler) override;
        bool BindCombinedSamplerTexture(uint32_t binding, const Ref<Rendering::Sampler>& sampler, const Ref<Rendering::Texture>& texture) override;
        bool BindBuffer(uint32_t binding, const Ref<Rendering::Buffer>& buffer, size_t offset, size_t size) override;

    private:
        Device* m_Device = nullptr;
        DescriptorPool* m_DescriptorPool = nullptr;
        VkDescriptorSet m_Handle = nullptr;
    };
}
