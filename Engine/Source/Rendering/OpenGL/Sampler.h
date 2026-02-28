#pragma once
#include "Common.h"
#include "Rendering/Sampler.h"

namespace Nova::OpenGL
{
    class RenderDevice;

    class Sampler final : public Nova::Sampler
    {
    public:
        bool Initialize(const SamplerCreateInfo& createInfo) override;
        void Destroy() override;

        uint32_t GetHandle() const { return m_Handle; }
    private:
        RenderDevice* m_Device = nullptr;
        uint32_t m_Handle = INVALID_HANDLE<uint32_t>;
    };
}
