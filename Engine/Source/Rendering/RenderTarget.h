#pragma once
#include "Multisample.h"
#include "RendererObject.h"
#include "Runtime/Format.h"
#include "Runtime/Flags.h"
#include "Runtime/LogCategory.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(RenderTarget, "RENDER TARGET")

namespace Nova
{
    class Renderer;
}

namespace Nova
{
    enum class RenderTargetAttachmentFlagBits
    {
        Color = BIT(0),
        Depth = BIT(1),
        Stencil = BIT(2),
        DepthStencil = Depth | Stencil
    };

    NOVA_DECLARE_FLAGS(RenderTargetAttachmentFlagBits, RenderTargetAttachmentFlags)

    struct RenderTargetAttachmentInfo
    {
        String Name;
        Format Format;
        RenderTargetAttachmentFlagBits AttachmentType;
    };

    struct RenderTargetCreateInfo
    {
        Array<RenderTargetAttachmentInfo> AttachmentInfos;
        u32 Width;
        u32 Height;
        u32 Depth;
        SampleCount SampleCount;
    };

    struct RenderTargetAttachment
    {
        String Name;
        Format Format;
    };

    class RenderTarget : public RendererObject<RenderTargetCreateInfo>
    {
    public:
        explicit RenderTarget(Renderer* Owner);

        virtual bool Resize(u32 Width, u32 Height) = 0;
        virtual void BeginRendering() = 0;
        virtual void EndRendering() = 0;

        u32 GetWidth() const;
        u32 GetHeight() const;
    protected:
        u32 m_Width = 0, m_Height = 0;
        SampleCount m_SampleCount;
        Array<RenderTargetAttachment*> m_Attachments;
    };
}
