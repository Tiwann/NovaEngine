#pragma once
#include "Core/Graphics/FrameBuffer.h"
#include "Core/NumericTypes.h"

typedef u32 GLenum;

namespace Nova
{
    class OpenGLFrameBuffer : public FrameBuffer
    {
    public:
        OpenGLFrameBuffer();
        ~OpenGLFrameBuffer() override;

        void Destroy() override;
        void Bind() override;
        void Unbind() override;
        void Resize(const Vector2& NewSize) override;
        AttachmentTexture CreateAttachment(FrameBufferAttachment Attachment) override;
        void RemoveAttachment(const AttachmentTexture& Attachment) override;
        bool Validate() override;


        static GLenum GetOpenGLAttachment(FrameBufferAttachment Attachment);

    protected:
        u32 m_Handle;
    };
}
