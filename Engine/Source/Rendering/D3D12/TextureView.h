#pragma once
#include "Rendering/TextureView.h"
#include <cstdint>

namespace Nova::D3D12
{
    class TextureView : public Nova::TextureView
    {
    public:
        bool Initialize(const TextureViewCreateInfo& createInfo) override;
        void Destroy() override;
    private:
        size_t cpuHandle = 0;
        uint64_t gpuHandle = 0;
    };
}