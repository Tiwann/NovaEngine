#pragma once
#include "Runtime/Object.h"
#include <cstdint>

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

        virtual bool BindTexture(uint32_t binding, const Texture& texture) = 0;
        virtual bool BindSampler(uint32_t binding, const Sampler& sampler) = 0;
        virtual bool BindCombinedSamplerTexture(uint32_t binding, const Sampler& sampler, const Texture& texture) = 0;
        virtual bool BindBuffer(uint32_t binding, const Buffer& buffer, size_t offset, size_t size) = 0;
    };
}
