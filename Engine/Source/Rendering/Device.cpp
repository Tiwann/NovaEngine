#include "Device.h"
#include "Vulkan/Device.h"
#include "ComputePipeline.h"
#include "Sampler.h"

#ifdef NOVA_HAS_D3D12
#include "D3D12/Device.h"
#endif

#ifdef NOVA_HAS_OPENGL
#include "OpenGL/Device.h"
#endif

namespace Nova
{


    StringView Device::GetDeviceVendor() const
    {
        return m_DeviceVendor;
    }

    bool Device::HasVSync() const
    {
        return m_VSync;
    }

    Ref<Device> CreateRenderDevice(const DeviceType type, const DeviceCreateInfo& createInfo)
    {
        Device* device = nullptr;
        switch (type)
        {
        case DeviceType::Null: return nullptr;
        case DeviceType::Vulkan:
            {
                device = new Vulkan::Device();
                if (!device->Initialize(createInfo))
                {
                    delete device;
                    return nullptr;
                }
            }
            break;
#ifdef NOVA_HAS_D3D12
        case DeviceType::D3D12:
            {
                device = new D3D12::Device();
                if (!device->Initialize(createInfo))
                {
                    delete device;
                    return nullptr;
                }
            }
            break;
#endif
        }
        return Ref(device);
    }

    Device::Device() : Object("Rendering Device")
    {
    }

    Ref<Fence> Device::CreateFence()
    {
        return CreateFence({this, FenceCreateFlagBits::None});
    }

    Ref<Buffer> Device::CreateBuffer(const BufferUsage usage, const size_t size)
    {
        const BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
        .WithUsage(usage)
        .WithSize(size);
        return CreateBuffer(bufferCreateInfo);
    }

    Ref<Texture> Device::CreateTexture(TextureUsageFlags usage, const uint32_t width, const uint32_t height, const Format format)
    {
        const TextureCreateInfo createInfo = TextureCreateInfo()
        .WithUsageFlags(usage)
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

    Ref<Sampler> Device::CreateSampler()
    {
        const SamplerCreateInfo createInfo = SamplerCreateInfo()
        .WithAddressMode(SamplerAddressMode::Repeat)
        .WithFilter(Filter::Linear, Filter::Linear);
        return CreateSampler(createInfo);
    }

    Ref<Sampler> Device::GetOrCreateSampler(const SamplerCreateInfo& createInfo)
    {
        if (!m_Samplers.Contains(createInfo))
        {
            Ref<Sampler> sampler = CreateSampler(createInfo);
            m_Samplers[createInfo] = sampler;
            return sampler;
        }

        return m_Samplers[createInfo];
    }
}
