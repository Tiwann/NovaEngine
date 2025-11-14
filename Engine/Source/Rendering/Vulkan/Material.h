#pragma once
#include "Rendering/Material.h"

namespace Nova::Vulkan
{
    class Material final : public Rendering::Material
    {
    public:
        bool Initialize(const Rendering::MaterialCreateInfo& createInfo) override;
        void Destroy() override;

        void SetSampler(StringView name, Ref<Rendering::Sampler> sampler) override;
        void SetTexture(StringView name, Ref<Rendering::Texture> texture) override;
        void SetSamplerAndTexture(StringView name, Ref<Rendering::Sampler> sampler, Ref<Rendering::Texture> texture) override;
        void SetBuffer(StringView name, Ref<Rendering::Buffer> buffer) override;
    };
}