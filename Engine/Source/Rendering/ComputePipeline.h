#pragma once
#include <cstdint>

typedef uint32_t VkPipelineCreateFlags;

namespace Nova::Rendering
{
    class Device;

    struct ComputePipelineCreateInfo
    {
        Device* device;
        VkPipelineCreateFlags flags;
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