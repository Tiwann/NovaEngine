#include "CommandPool.h"
#include "Device.h"
#include <vulkan/vulkan.h>


namespace Nova::Vulkan
{
    bool CommandPool::Initialize(const Rendering::CommandPoolCreateInfo& createInfo)
    {
        Device* device = dynamic_cast<Device*>(createInfo.device);
        const VkDevice deviceHandle = device->GetHandle();
        const Queue* graphicsQueue = device->GetGraphicsQueue();
        VkCommandPoolCreateInfo poolCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        poolCreateInfo.flags = createInfo.flags;
        poolCreateInfo.queueFamilyIndex = graphicsQueue->GetIndex();

        if (vkCreateCommandPool(deviceHandle, &poolCreateInfo, nullptr, &m_Handle) != VK_SUCCESS)
            return false;

        m_Device = device;
        return true;
    }

    void CommandPool::Destroy()
    {
        const VkDevice deviceHandle = m_Device->GetHandle();
        vkDestroyCommandPool(deviceHandle, m_Handle, nullptr);
    }

    VkCommandPool CommandPool::GetHandle()
    {
        return m_Handle;
    }

    const VkCommandPool* CommandPool::GetHandle() const
    {
        return &m_Handle;
    }

}
