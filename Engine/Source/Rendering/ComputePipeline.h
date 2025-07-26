#pragma once

namespace Nova::Rendering
{
    class Device;
    class ShaderModule;

    struct ComputePipelineCreateInfo
    {
        Device* device = nullptr;
        ShaderModule* shaderModule = nullptr;
        void* pipelineLayout = nullptr;
    };

    class ComputePipeline
    {
    public:
        ComputePipeline() = default;
        virtual ~ComputePipeline() = default;

        virtual bool Initialize(const ComputePipelineCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        Device* GetDevice() const { return m_Device; }
    protected:
        Device* m_Device = nullptr;
    };
}