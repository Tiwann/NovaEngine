#pragma once
#include "DescriptorHeap.h"
#include "Rendering/Sampler.h"

namespace Nova::D3D12
{
    class RenderDevice;
    class Sampler : public Nova::Sampler
    {
    public:
        bool Initialize(const SamplerCreateInfo& createInfo) override;
        void Destroy() override;
    private:
        DescriptorHandle m_Handle = nullptr;
        RenderDevice* m_Device = nullptr;
    };
}
