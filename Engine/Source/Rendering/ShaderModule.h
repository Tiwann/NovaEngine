#pragma once
#include "Containers/Array.h"
#include "Rendering/ShaderStage.h"
#include <cstdint>


namespace Nova
{
    class Device;

    struct ShaderModuleCreateInfo
    {
        Device* device = nullptr;
        const uint32_t* code = nullptr;
        size_t codeSize = 0;
        ShaderStageFlagBits stage = ShaderStageFlagBits::None;
    };

    class ShaderModule
    {
    public:
        ShaderModule() = default;
        virtual ~ShaderModule() = default;
        virtual bool Initialize(const ShaderModuleCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;
        ShaderStageFlagBits GetStage() const { return m_Stage; }
    protected:
        ShaderStageFlagBits m_Stage = ShaderStageFlagBits::None;
    };
}
