#pragma once

namespace Nova
{
    class RenderDevice;
    class Shader;

    struct ComputePipelineCreateInfo
    {
        RenderDevice* device = nullptr;
        const Shader* shader = nullptr;

        ComputePipelineCreateInfo& WithDevice(RenderDevice* inDevice) { this->device = inDevice; return *this; }
        ComputePipelineCreateInfo& WithShader(const Shader* inShader) { this->shader = inShader; return *this; }
    };

    class ComputePipeline
    {
    public:
        ComputePipeline() = default;
        virtual ~ComputePipeline() = default;

        virtual bool Initialize(const ComputePipelineCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        RenderDevice* GetDevice() const { return m_Device; }
    protected:
        RenderDevice* m_Device = nullptr;
    };
}