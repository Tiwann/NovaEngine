#include "ShaderBindingSet.h"
#include "DescriptorPool.h"

namespace Nova::Vulkan
{
    bool ShaderBindingSet::Initialize(const Rendering::DescriptorPool& descriptorPool, const Rendering::ShaderBindingSetLayout& bindingSetLayout)
    {
        const DescriptorPool& pool = (const DescriptorPool&)descriptorPool;
        m_Handle = pool.AllocateDescriptorSet(bindingSetLayout);
        if (!m_Handle)
            return false;
        return true;
    }

    void ShaderBindingSet::Destroy()
    {

    }

    VkDescriptorSet ShaderBindingSet::GetHandle() const
    {
        return m_Handle;
    }
}
