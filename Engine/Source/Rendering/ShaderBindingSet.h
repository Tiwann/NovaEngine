#pragma once
#include "Runtime/Object.h"
#include "Runtime/Ref.h"
#include <cstdint>

#include "ShaderBindingSetLayout.h"


namespace Nova::Rendering
{
    class Device;
    class Buffer;
    class Sampler;
    class Texture;
    class ShaderBindingSetLayout;
    class DescriptorPool;

    struct ShaderBindingSetCreateInfo
    {
        Device* device = nullptr;
        DescriptorPool* pool = nullptr;
        const ShaderBindingSetLayout* layout = nullptr;
    };

    class ShaderBindingSet : public Object
    {
    public:
        ShaderBindingSet() : Object("Shader Binding Set") {}
        ~ShaderBindingSet() override = default;

        virtual bool Initialize(const ShaderBindingSetCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        virtual bool BindTexture(uint32_t binding, const Ref<Texture>& texture) = 0;
        virtual bool BindSampler(uint32_t binding, const Ref<Sampler>& sampler) = 0;
        virtual bool BindCombinedSamplerTexture(uint32_t binding, const Ref<Sampler>& sampler, const Ref<Texture>& texture) = 0;
        virtual bool BindBuffer(uint32_t binding, const Ref<Buffer>& buffer, size_t offset, size_t size) = 0;

        const ShaderBindingSetLayout* GetBindingSetLayout() const { return m_BindingSetLayout; }
        uint32_t GetSetIndex() const { return m_BindingSetLayout->GetSetIndex(); }
    protected:
        ShaderBindingSetLayout* m_BindingSetLayout = nullptr;
    };
}
