#include "DescriptorPool.h"
#include "GraphicsApi.h"
#include "Renderer.h"

#include "Platform/Vulkan/VulkanDescriptorSet.h"

namespace Nova
{
    DescriptorSet* DescriptorPool::AllocateDescriptorSet(const DescriptorSetAllocateInfo& AllocateInfo)
    {
        DescriptorSet* Set = nullptr;

        switch (m_Owner->GetGraphicsApi())
        {
        case GraphicsApi::None: return nullptr;
        case GraphicsApi::OpenGL: return nullptr;
        case GraphicsApi::Vulkan: Set = new VulkanDescriptorSet(this); break;
        case GraphicsApi::D3D12: return nullptr;
        default: return nullptr;
        }

        if (!Set->Initialize(AllocateInfo))
        {
            delete Set;
            return nullptr;
        }
        return Set;
    }
}
