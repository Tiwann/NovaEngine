#include "ComputePipeline.h"
#include "Device.h"

#include <vulkan/vulkan.h>

namespace Nova::Vulkan
{
    bool ComputePipeline::Initialize(const Rendering::ComputePipelineCreateInfo& createInfo)
    {
        m_Device = static_cast<Device*>(createInfo.device);

        VkComputePipelineCreateInfo computeCreateInfo = { VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
        return true;
    }

    void ComputePipeline::Destroy()
    {
    }

    VkPipeline ComputePipeline::GetHandle() const
    {
        return m_Handle;
    }

    const VkPipeline* ComputePipeline::GetHandlePtr() const
    {
        return &m_Handle;
    }
}
