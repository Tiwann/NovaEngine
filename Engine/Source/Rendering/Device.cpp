#include "Device.h"
#include "Vulkan/Device.h"

namespace Nova::Rendering
{
    Device* Device::Create(const DeviceType type, const DeviceCreateInfo& createInfo)
    {
        Device* device = nullptr;
        switch (type)
        {
        case DeviceType::Vulkan:
            {
                device = new Vulkan::Device();
                if (!device->Initialize(createInfo))
                {
                    delete device;
                    return nullptr;
                }
            }
        default: return nullptr;
        }
    }
}
