#include "Device.h"

namespace Nova::Null
{
    bool Device::Initialize(const DeviceCreateInfo& createInfo)
    {
        return true;
    }

    void Device::Destroy()
    {
    }

    bool Device::BeginFrame()
    {
        return true;
    }

    void Device::EndFrame()
    {
    }

    void Device::Present()
    {
    }

    void Device::WaitIdle() const
    {
    }

    void Device::SetName(StringView name)
    {
        (void)name;
    }

    DeviceType Device::GetDeviceType()
    {
        return DeviceType::Null;
    }

    Ref<Surface> Device::CreateSurface(const SurfaceCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }

    Ref<Texture> Device::CreateTexture(const TextureCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }

    Ref<Texture> Device::CreateTextureUnitialized()
    {
        return nullptr;
    }

    Ref<Sampler> Device::CreateSampler(const SamplerCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }

    Ref<Buffer> Device::CreateBuffer(const BufferCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }

    Ref<Shader> Device::CreateShader(const ShaderCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }

    Ref<GraphicsPipeline> Device::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }

    Ref<ComputePipeline> Device::CreateComputePipeline(const ComputePipelineCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }

    Ref<Material> Device::CreateMaterial(const MaterialCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }

    Ref<Fence> Device::CreateFence(const FenceCreateInfo& createInfo)
    {
        (void)createInfo;
        return nullptr;
    }

    uint32_t Device::GetImageCount() const
    {
        return 0;
    }

    Ref<Nova::RenderTarget> Device::CreateRenderTarget(const RenderTargetCreateInfo& createInfo)
    {
        return nullptr;
    }
}
