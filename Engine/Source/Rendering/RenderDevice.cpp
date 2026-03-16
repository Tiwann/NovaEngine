#include "RenderDevice.h"
#include "ComputePipeline.h"
#include "Sampler.h"
#include "Buffer.h"
#include "Material.h"
#include "RenderTarget.h"
#include "ResourceBarrier.h"
#include "Shader.h"
#include "Runtime/Common.h"

#ifdef NOVA_HAS_VULKAN
#include "Vulkan/RenderDevice.h"
#endif

#ifdef NOVA_HAS_D3D12
#include "D3D12/RenderDevice.h"
#endif

#ifdef NOVA_HAS_OPENGL
#include "OpenGL/RenderDevice.h"
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

    Ref<RenderDevice> RenderDevice::GetInstance()
    {
        return Ref(s_Instance);
    }

    Ref<RenderDevice> CreateRenderDevice(const RenderDeviceType type, const RenderDeviceCreateInfo& createInfo)
    {
        RenderDevice* device = nullptr;
        switch (type)
        {
        case RenderDeviceType::Null: return nullptr;
#ifdef NOVA_HAS_VULKAN
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
#endif
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
#ifdef NOVA_HAS_OPENGL
        case RenderDeviceType::OpenGL:
            {
                device = new OpenGL::RenderDevice();
                if (!device->Initialize(createInfo))
                {
                    delete device;
                    return nullptr;
                }
            }
            break;
#endif
        default: return nullptr;
        }
        return Ref(device);
    }

    void RenderDevice::Destroy()
    {
        for (auto& [_, sampler] : m_Samplers)
            sampler->Destroy();
    }

    Ref<Nova::RenderTarget> RenderDevice::CreateRenderTarget(const RenderTargetCreateInfo& createInfo)
    {
        RenderTarget* renderTarget = new RenderTarget();
        if (!renderTarget->Initialize(createInfo))
        {
            delete renderTarget;
            return nullptr;
        }
        return Ref(renderTarget);
    }

    Ref<Fence> RenderDevice::CreateFence()
    {
        FenceCreateInfo createInfo;
        createInfo.device = this;
        createInfo.flags = FenceCreateFlagBits::None;
        return CreateFence(createInfo);
    }

    Ref<Buffer> RenderDevice::CreateBuffer(const BufferUsage usage, const size_t size)
    {
        const BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
        .WithUsage(usage)
        .WithSize(size);
        return CreateBuffer(bufferCreateInfo);
    }

    Ref<Texture> RenderDevice::CreateTexture(const TextureUsageFlags usage, const uint32_t width, const uint32_t height, const Format format)
    {
        TextureCreateInfo createInfo;
        createInfo.width = width;
        createInfo.height = height;
        createInfo.depth = 1;
        createInfo.format = format;
        createInfo.usageFlags = usage;
        createInfo.sampleCount = 1;
        createInfo.mipCount = 1;
        return CreateTexture(createInfo);
    }

    Ref<Material> RenderDevice::CreateMaterial(Ref<Shader> material)
    {
        const MaterialCreateInfo materialCreateInfo = MaterialCreateInfo().WithShader(material);
        return CreateMaterial(materialCreateInfo);
    }

    Ref<ComputePipeline> RenderDevice::CreateComputePipeline(Ref<Shader> shader)
    {
        const ComputePipelineCreateInfo createInfo = ComputePipelineCreateInfo().WithShader(shader.Get());
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

    void RenderDevice::ImmediateTextureBarrier(RenderDevice* device, const TextureBarrier& barrier)
    {
        if (!device) return;

        Ref<CommandBuffer> cmdBuffer = device->CreateCommandBuffer();
        if (!cmdBuffer) return;
        NOVA_DEFER(cmdBuffer, &CommandBuffer::Free);

        Ref<Fence> fence = device->CreateFence();
        if (!fence) return;
        NOVA_DEFER(fence, &Fence::Destroy);

        cmdBuffer->Begin({CommandBufferUsageFlagBits::OneTimeSubmit});
        cmdBuffer->TextureBarrier(barrier);
        cmdBuffer->End();

        const Queue* queue = device->GetGraphicsQueue();
        if (!queue) return;
        queue->Submit(cmdBuffer, nullptr, nullptr, fence);
        fence->Wait(FENCE_WAIT_INFINITE);
    }
}
