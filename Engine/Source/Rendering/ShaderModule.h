#pragma once
#include "Containers/Array.h"
#include "Rendering/ShaderStage.h"
#include <cstdint>


namespace Nova::Rendering
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

        template<typename ShaderModuleType> requires IsBaseOf<ShaderModule, ShaderModuleType>::value
        static ShaderModuleType Create(Device& device, ShaderStageFlagBits stage, const Array<uint32_t>& spirvCode)
        {
            Rendering::ShaderModuleCreateInfo createInfo;
            createInfo.device = &device;
            createInfo.stage = stage;
            createInfo.code = spirvCode.Data();
            createInfo.codeSize = spirvCode.Size();

            ShaderModuleType shaderModule;
            shaderModule.Initialize(createInfo);
            return std::move(shaderModule);
        }

        ShaderStageFlags GetStageFlags() const { return m_Stage; }
    protected:
        ShaderStageFlags m_Stage = ShaderStageFlagBits::None;
    };
}
