#pragma once

namespace Nova::Rendering
{
    class Device;
    class Shader;

    struct ComputePipelineCreateInfo
    {
        Device* device = nullptr;
        Shader* shader = nullptr;

        ComputePipelineCreateInfo& withDevice(Device* device) { this->device = device; return *this; }
        ComputePipelineCreateInfo& withShader(Shader* shader) { this->shader = shader; return *this; }
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