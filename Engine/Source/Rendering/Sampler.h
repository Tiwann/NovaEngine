#pragma once
#include "CompareOperation.h"
#include "Filter.h"
#include "SamplerAddressMode.h"


namespace Nova::Rendering
{
    class Device;

    struct SamplerCreateInfo
    {
        Device* device = nullptr;
        SamplerAddressMode addressModeU = SamplerAddressMode::Repeat;
        SamplerAddressMode addressModeV = SamplerAddressMode::Repeat;
        SamplerAddressMode addressModeW = SamplerAddressMode::Repeat;
        Filter minFilter = Filter::Nearest;
        Filter magFilter = Filter::Nearest;
        bool anisotropyEnable = false;
        bool compareEnable = false;
        CompareOperation compareOp = CompareOperation::Always;
        bool unnormalizedCoordinates = false;
        float minLod = 0.0f;
        float maxLod = 1.0f;
        Filter mipmapFilter = Filter::Nearest;
    };

    class Sampler
    {
    public:
        Sampler() = default;
        virtual ~Sampler() = default;

        virtual bool Initialize(const SamplerCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;
    protected:
        SamplerAddressMode m_AddressModeU = SamplerAddressMode::Repeat;
        SamplerAddressMode m_AddressModeV = SamplerAddressMode::Repeat;
        SamplerAddressMode m_AddressModeW = SamplerAddressMode::Repeat;
        Filter m_MinFilter = Filter::Nearest;
        Filter m_MagFilter = Filter::Nearest;
        bool m_AnisotropyEnable = false;
        bool m_CompareEnable = false;
        CompareOperation m_CompareOp = CompareOperation::Always;
        bool m_UnnormalizedCoordinates = false;
        float m_MinLod = 0.0f;
        float m_MaxLod = 1.0f;
        Filter m_MipmapFilter = Filter::Nearest;
    };
}
