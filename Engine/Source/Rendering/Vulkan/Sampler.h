#pragma once
#include "Rendering/Sampler.h"

typedef struct VkSampler_T* VkSampler;

namespace Nova::Vulkan
{
    class RenderDevice;

    class Sampler final : public Nova::Sampler
    {
    public:
        bool Initialize(const SamplerCreateInfo& createInfo) override;
        void Destroy() override;

        VkSampler GetHandle() const;
    private:
        RenderDevice* m_Device = nullptr;
        VkSampler m_Handle = nullptr;
    };
}
