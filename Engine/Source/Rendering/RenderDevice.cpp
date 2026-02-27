#include "RenderDevice.h"
#include "Vulkan/RenderDevice.h"
#include "ComputePipeline.h"
#include "Sampler.h"

#ifdef NOVA_HAS_D3D12
#include "D3D12/RenderDevice.h"
#endif

#ifdef NOVA_HAS_OPENGL
#include "OpenGL/Device.h"
#endif

namespace Nova
{


    StringView RenderDevice::GetDeviceVendor() const
    {
        return m_DeviceVendor;
    }

    bool RenderDevice::HasVSync() const
    {
        return m_VSync;
    }

    Ref<RenderDevice> CreateRenderDevice(const RenderDeviceType type, const DeviceCreateInfo& createInfo)
    {
        RenderDevice* device = nullptr;
        switch (type)
        {
        case RenderDeviceType::Null: return nullptr;
        case RenderDeviceType::Vulkan:
            {
                device = new Vulkan::RenderDevice();
                if (!device->Initialize(createInfo))
                {
                    delete device;
                    return nullptr;
                }
            }
            break;
#ifdef NOVA_HAS_D3D12
        case RenderDeviceType::D3D12:
            {
                device = new D3D12::RenderDevice();
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

    RenderDevice::RenderDevice() : Object("Rendering Device")
    {
    }

    void RenderDevice::Destroy()
    {
        for (auto& [_, sampler] : m_Samplers)
            sampler->Destroy();
    }

    Ref<Fence> RenderDevice::CreateFence()
    {
        return CreateFence({this, FenceCreateFlagBits::None});
    }

    Ref<Buffer> RenderDevice::CreateBuffer(const BufferUsage usage, const size_t size)
    {
        const BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
        .WithUsage(usage)
        .WithSize(size);
        return CreateBuffer(bufferCreateInfo);
    }

    Ref<Texture> RenderDevice::CreateTexture(TextureUsageFlags usage, const uint32_t width, const uint32_t height, const Format format)
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

    Ref<Material> RenderDevice::CreateMaterial(Ref<Shader> material)
    {
        const MaterialCreateInfo materialCreateInfo = MaterialCreateInfo().WithShader(material);
        return CreateMaterial(materialCreateInfo);
    }

    Ref<ComputePipeline> RenderDevice::CreateComputePipeline(Ref<Shader> shader)
    {
        const ComputePipelineCreateInfo createInfo = ComputePipelineCreateInfo().WithShader(shader);
        return CreateComputePipeline(createInfo);
    }

    Ref<Sampler> RenderDevice::CreateSampler()
    {
        const SamplerCreateInfo createInfo = SamplerCreateInfo()
        .WithAddressMode(SamplerAddressMode::Repeat)
        .WithFilter(Filter::Linear, Filter::Linear);
        return CreateSampler(createInfo);
    }

    Ref<Sampler> RenderDevice::GetOrCreateSampler(const SamplerCreateInfo& createInfo)
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
