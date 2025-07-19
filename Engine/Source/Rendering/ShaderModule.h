#pragma once
#include "Rendering/ShaderStage.h"
#include <cstdint>

namespace Nova::Rendering
{
    class Device;

    struct ShaderModuleCreateInfo
    {
        Device* device = nullptr;
        uint32_t* code = nullptr;
        size_t codeSize = 0;
        ShaderStageFlags stage = ShaderStageFlagBits::None;
    };

    class ShaderModule
    {
    public:
        ShaderModule() = default;
        virtual ~ShaderModule() = default;

        virtual bool Initialize(const ShaderModuleCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        ShaderStageFlags GetStageFlags() const { return m_Stage; }
    protected:
        ShaderStageFlags m_Stage = ShaderStageFlagBits::None;
    };
}