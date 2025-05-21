#pragma once
#include "Multisample.h"
#include "RendererObject.h"
#include "Runtime/Format.h"
#include "Runtime/LogCategory.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(RenderTarget, "RENDER TARGET")

namespace Nova
{
    class Renderer;
}

namespace Nova
{
    struct RenderTargetCreateInfo
    {
        u32 Width;
        u32 Height;
        u32 Depth;
        Format ColorFormat;
        Format DepthFormat;
        SampleCount SampleCount;
    };

    class RenderTarget : public RendererObject<RenderTargetCreateInfo>
    {
    public:
        explicit RenderTarget(Renderer* Owner);

        virtual bool Resize(u32 Width, u32 Height) = 0;

        u32 GetWidth() const;
        u32 GetHeight() const;
    protected:
        u32 m_Width = 0, m_Height = 0;
        SampleCount m_SampleCount;
    };
}
