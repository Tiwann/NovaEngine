#include "RenderDevice.h"

namespace Nova::Null
{
    bool RenderDevice::Initialize(const DeviceCreateInfo& createInfo)
    {
        return true;
    }

    void RenderDevice::Destroy()
    {
    }

    bool RenderDevice::BeginFrame()
    {
        return true;
    }

    void RenderDevice::EndFrame()
    {
    }

    void RenderDevice::Present()
    {
    }

    void RenderDevice::WaitIdle() const
    {
    }

    void RenderDevice::SetName(StringView name)
    {
        (void)name;
    }

    RenderDeviceType RenderDevice::GetDeviceType()
    {
        return RenderDeviceType::Null;
    }

    Ref<Surface> RenderDevice::CreateSurface(const SurfaceCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }

    Ref<Texture> RenderDevice::CreateTexture(const TextureCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }

    Ref<Texture> RenderDevice::CreateTextureUnitialized()
    {
        return nullptr;
    }

    Ref<Sampler> RenderDevice::CreateSampler(const SamplerCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }

    Ref<Buffer> RenderDevice::CreateBuffer(const BufferCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }

    Ref<Shader> RenderDevice::CreateShader(const ShaderCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }

    Ref<GraphicsPipeline> RenderDevice::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }

    Ref<ComputePipeline> RenderDevice::CreateComputePipeline(const ComputePipelineCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }

    Ref<Material> RenderDevice::CreateMaterial(const MaterialCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }

    Ref<Fence> RenderDevice::CreateFence(const FenceCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }

    uint32_t RenderDevice::GetImageCount() const
    {
        return 0;
    }

    Ref<RenderTarget> RenderDevice::CreateRenderTarget(const RenderTargetCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }
}
