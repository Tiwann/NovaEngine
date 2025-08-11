#pragma once
#include "Device.h"
#include "RenderTarget.h"
#include "Vulkan/RenderTarget.h"


namespace Nova::Rendering
{
    uint32_t RenderTarget::GetWidth() const
    {
        return m_Width;
    }

    uint32_t RenderTarget::GetHeight() const
    {
        return m_Height;
    }

    uint32_t RenderTarget::GetDepth() const
    {
        return m_Depth;
    }

    Format RenderTarget::GetColorFormat() const
    {
        return m_ColorFormat;
    }

    Format RenderTarget::GetDepthFormat() const
    {
        return m_DepthFormat;
    }

    uint32_t RenderTarget::GetSampleCount() const
    {
        return m_SampleCount;
    }

    uint32_t RenderTarget::GetImageCount() const
    {
        return m_ImageCount;
    }

    Ref<RenderTarget> CreateRenderTarget(const RenderTargetCreateInfo& createInfo)
    {
        if (!createInfo.device) return nullptr;
        RenderTarget* renderTarget = nullptr;
        switch (createInfo.device->GetDeviceType())
        {
        case DeviceType::Unknown: return nullptr;
        case DeviceType::Vulkan:
            {
                renderTarget = new Vulkan::RenderTarget();
                if (!renderTarget->Initialize(createInfo))
                {
                    delete renderTarget;
                    return nullptr;
                }
            }
        }
        return Ref(renderTarget);
    }
}
