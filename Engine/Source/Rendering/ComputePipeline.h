#pragma once

namespace Nova
{
    class RenderDevice;
    class Shader;

    struct ComputePipelineCreateInfo
    {
        RenderDevice* device = nullptr;
        Shader* shader = nullptr;

        ComputePipelineCreateInfo& WithDevice(RenderDevice* device) { this->device = device; return *this; }
        ComputePipelineCreateInfo& WithShader(Shader* shader) { this->shader = shader; return *this; }
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