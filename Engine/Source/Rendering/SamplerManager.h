#pragma once
#include "Sampler.h"
#include "Containers/Map.h"
#include "Runtime/Object.h"
#include "Runtime/Ref.h"

namespace Nova
{
    class Device;

    class SamplerManager final : public Object
    {
    public:
        explicit SamplerManager() = default;
        bool Initialize(Device* device);
        void Destroy();
        Ref<Sampler> GetOrCreateSampler(const SamplerCreateInfo& createInfo);
    private:
        Device* m_Device = nullptr;
        Map<SamplerCreateInfo, Ref<Sampler>> m_Data;
    };
}
