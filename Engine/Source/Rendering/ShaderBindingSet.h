#pragma once
#include "Runtime/Object.h"
#include "Runtime/Ref.h"
#include "ShaderBindingSetLayout.h"
#include <cstdint>


namespace Nova
{
    class RenderDevice;
    class Buffer;
    class Sampler;
    class Texture;
    class ShaderBindingSetLayout;
    class DescriptorPool;

    struct ShaderBindingSetCreateInfo
    {
        RenderDevice* device = nullptr;
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

        virtual bool BindTextures(uint32_t binding, const Nova::Texture* const* textures, size_t textureCount, BindingType bindingType) = 0;
        virtual bool BindTexture(uint32_t binding, const Nova::Texture& texture, BindingType bindingType) = 0;
        virtual bool BindSampler(uint32_t binding, const Nova::Sampler& sampler) = 0;
        virtual bool BindCombinedSamplerTexture(uint32_t binding, const Nova::Sampler& sampler, const Nova::Texture& texture) = 0;
        virtual bool BindCombinedSamplerTextures(uint32_t binding, const Nova::Sampler& sampler, const Nova::Texture* const* textures, size_t textureCount) = 0;
        virtual bool BindBuffer(uint32_t binding, const Nova::Buffer& buffer, size_t offset, size_t size) = 0;

        const ShaderBindingSetLayout* GetBindingSetLayout() const { return m_BindingSetLayout; }
        uint32_t GetSetIndex() const { return m_BindingSetLayout->GetSetIndex(); }
    protected:
        ShaderBindingSetLayout* m_BindingSetLayout = nullptr;
    };
}
