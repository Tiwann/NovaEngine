#pragma once
#include "Rendering/ShaderModule.h"

typedef struct VkShaderModule_T* VkShaderModule;

namespace Nova::Vulkan
{
    class Device;

    class ShaderModule : public Rendering::ShaderModule
    {
    public:
        bool Initialize(const Rendering::ShaderModuleCreateInfo& createInfo) override;
        void Destroy() override;

        VkShaderModule GetHandle() const;
        // VkPipelineShaderStageCreateInfo GetShaderStageCreateInfo() const;
    private:
        Device* m_Device = nullptr;
        VkShaderModule m_Handle = nullptr;

    };
}
