#pragma once
#include "Rendering/Material.h"

namespace Nova::Vulkan
{
    class Material final : public Nova::Material
    {
    public:
        bool Initialize(const MaterialCreateInfo& createInfo) override;
        void Destroy() override;

        void SetSampler(StringView name, Ref<Nova::Sampler> sampler) override;
        void SetTexture(StringView name, Ref<Nova::Texture> texture) override;
        void SetSamplerAndTexture(StringView name, Ref<Nova::Sampler> sampler, Ref<Nova::Texture> texture) override;
        void SetBuffer(StringView name, Ref<Nova::Buffer> buffer, size_t offset, size_t size) override;
    };
}