#pragma once
#include "Containers/Array.h"
#include "Rendering/ShaderStage.h"
#include <cstdint>


namespace Nova
{
    class RenderDevice;

    struct ShaderModuleCreateInfo
    {
        RenderDevice* device = nullptr;
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
        static ShaderModuleType Create(RenderDevice& device, ShaderStageFlagBits stage, const Array<uint32_t>& spirvCode)
        {
            ShaderModuleCreateInfo createInfo;
            createInfo.device = &device;
            createInfo.stage = stage;
            createInfo.code = spirvCode.Data();
            createInfo.codeSize = spirvCode.Size();

            ShaderModuleType shaderModule;
            shaderModule.Initialize(createInfo);
            return std::move(shaderModule);
        }

        ShaderStageFlagBits GetStage() const { return m_Stage; }
    protected:
        ShaderStageFlagBits m_Stage = ShaderStageFlagBits::None;
    };
}
