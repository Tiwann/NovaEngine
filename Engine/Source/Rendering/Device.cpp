#include "Device.h"
#include "Vulkan/Device.h"

#include "ComputePipeline.h"

namespace Nova
{


    StringView Device::GetDeviceVendor() const
    {
        return m_DeviceVendor;
    }

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

    Ref<Buffer> Device::CreateBuffer(const BufferUsage usage, const size_t size)
    {
        const BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
        .WithUsage(usage)
        .WithSize(size);
        return CreateBuffer(bufferCreateInfo);
    }

    Ref<Texture> Device::CreateTexture(const TextureUsageFlags usageFlags, const uint32_t width, const uint32_t height, const Format format)
    {
        const TextureCreateInfo createInfo = TextureCreateInfo()
        .WithFlags(usageFlags)
        .WithWidth(width)
        .WithHeight(height)
        .WithDepth(1)
        .WithFormat(format)
        .WithMips(1)
        .WithSampleCount(1);
        return CreateTexture(createInfo);
    }

    Ref<Material> Device::CreateMaterial(Ref<Shader> material)
    {
        const MaterialCreateInfo materialCreateInfo = MaterialCreateInfo().WithShader(material);
        return CreateMaterial(materialCreateInfo);
    }

    Ref<ComputePipeline> Device::CreateComputePipeline(Ref<Shader> shader)
    {
        const ComputePipelineCreateInfo createInfo = ComputePipelineCreateInfo().WithShader(shader);
        return CreateComputePipeline(createInfo);
    }
}
