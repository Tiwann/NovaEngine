#pragma once
#include "Rendering/ShaderModule.h"

typedef struct VkShaderModule_T* VkShaderModule;

namespace Nova::Vulkan
{
    class RenderDevice;

    class ShaderModule final : public Nova::ShaderModule
    {
    public:
        bool Initialize(const ShaderModuleCreateInfo& createInfo) override;
        void Destroy() override;

        VkShaderModule GetHandle() const;
        // VkPipelineShaderStageCreateInfo GetShaderStageCreateInfo() const;
    private:
        RenderDevice* m_Device = nullptr;
        VkShaderModule m_Handle = nullptr;

    };
}
