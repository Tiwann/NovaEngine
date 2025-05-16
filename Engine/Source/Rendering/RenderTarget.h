#pragma once
#include "Runtime/Format.h"
#include "Runtime/Object.h"
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
    };

    struct RenderTargetAttachment
    {
        String Name;
        Format Format;
    };

    class RenderTarget : public Object
    {
    public:
        explicit RenderTarget(Renderer* Owner);

        virtual bool Initialize(const RenderTargetCreateInfo& Info) = 0;
        virtual bool Resize(const u32 Width, const u32 Height) = 0;
        virtual void BeginRendering() = 0;
        virtual void EndRendering() = 0;
        virtual void Destroy() = 0;

        Renderer* GetOwner() const;
        u32 GetWidth() const;
        u32 GetHeight() const;
    protected:
        Renderer* m_Owner = nullptr;
        u32 m_Width = 0, m_Height = 0;
        Array<RenderTargetAttachment*> m_Attachments;
    };
}
