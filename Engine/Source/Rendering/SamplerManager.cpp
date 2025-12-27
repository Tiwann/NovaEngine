#include "SamplerManager.h"
#include "Device.h"

namespace Nova
{
    bool SamplerManager::Initialize(Device* device)
    {
        if (!device) return false;
        m_Device = device;
        return true;
    }

    void SamplerManager::Destroy()
    {
        for (auto& [_, sampler] : m_Data)
            sampler->Destroy();
    }

    Ref<Sampler> SamplerManager::GetOrCreateSampler(const SamplerCreateInfo& createInfo)
    {
        if (!m_Device) return nullptr;
        if (!m_Data.Contains(createInfo))
        {
            Ref<Sampler> sampler = m_Device->CreateSampler(createInfo);
            m_Data[createInfo] = sampler;
            return sampler;
        }

        return m_Data[createInfo];
    }
}
