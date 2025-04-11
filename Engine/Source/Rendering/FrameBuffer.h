#pragma once
#include "Containers/Array.h"
#include "Renderer.h"

namespace Nova
{
    class Texture2D;
    
    enum class FrameBufferAttachment
    {
        None,
        Color,
        Depth,
        Stencil
    };

    struct AttachmentTexture
    {
        FrameBufferAttachment Attachment;
        Texture2D* Texture;

        bool operator==(const AttachmentTexture& Other) const
        {
            return Attachment == Other.Attachment && Texture == Other.Texture;
        }
    };
    
    class FrameBuffer
    {
    public:
        FrameBuffer() = default;
        virtual ~FrameBuffer() = default;

        virtual void Destroy() = 0;
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
        virtual AttachmentTexture CreateAttachment(FrameBufferAttachment Attachment) = 0;
        virtual void RemoveAttachment(const AttachmentTexture& Attachment) = 0;
        virtual bool Validate() = 0;
        virtual void Resize(const Vector2& NewSize) = 0;

        AttachmentTexture& GetAttachment(Array<AttachmentTexture>::SizeType Index);
        const Array<AttachmentTexture>& GetAttachments() const;
        static FrameBuffer* Create(const GraphicsApi& GraphicsApi);
    protected:
        Array<AttachmentTexture> m_Attachments;
    };
}
