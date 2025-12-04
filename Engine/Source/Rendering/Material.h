#pragma once
#include "Runtime/Asset.h"
#include "Runtime/Ref.h"
#include "Containers/StringView.h"

namespace Nova
{
    class Device;
    class Shader;
    class ShaderBindingSet;
    class Texture;
    class Buffer;
    class Sampler;

    struct MaterialCreateInfo
    {
        Ref<Device> device = nullptr;
        Ref<Shader> shader = nullptr;

        MaterialCreateInfo& WithDevice(Device* inDevice) { this->device = inDevice; return *this; }
        MaterialCreateInfo& WithShader(Shader* inShader) { this->shader = inShader; return *this; }
    };

    class Material : public Asset
    {
    public:
        Material() = default;

        virtual bool Initialize(const MaterialCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        AssetType GetAssetType() const final { return AssetType::Material; }

        virtual void SetSampler(StringView name, Ref<Sampler> sampler) = 0;
        virtual void SetTexture(StringView name, Ref<Texture> texture) = 0;
        virtual void SetSamplerAndTexture(StringView name, Ref<Sampler> sampler, Ref<Texture> texture) = 0;
        virtual void SetBuffer(StringView name, Ref<Buffer> buffer, size_t offset, size_t size) = 0;

        const Ref<ShaderBindingSet>& GetBindingSet() const { return m_BindingSet; }

        Ref<Shader> GetShader() const { return m_Shader; }
    protected:
        Ref<Device> m_Device = nullptr;
        Ref<Shader> m_Shader = nullptr;
        Ref<ShaderBindingSet> m_BindingSet = nullptr;
    };
}
