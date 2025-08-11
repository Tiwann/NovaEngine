#include "Device.h"
#include "Vulkan/Device.h"

namespace Nova::Rendering
{
    Ref<Device> CreateRenderDevice(const DeviceType type, const DeviceCreateInfo& createInfo)
    {
        Device* device = nullptr;
        switch (type)
        {
        case DeviceType::Unknown: return nullptr;
        case DeviceType::Vulkan:
            {
                device = new Vulkan::Device();
                if (!device->Initialize(createInfo))
                {
                    delete device;
                    return nullptr;
                }
            }
        }
        return Ref(device);
    }
}
