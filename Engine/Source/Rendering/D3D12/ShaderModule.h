#pragma once
#include "Rendering/ShaderModule.h"

namespace Nova::D3D12
{
    class ShaderModule : public Nova::ShaderModule
    {
    public:
        bool Initialize(const ShaderModuleCreateInfo& createInfo) override;
        void Destroy() override;
    private:

    };
}
