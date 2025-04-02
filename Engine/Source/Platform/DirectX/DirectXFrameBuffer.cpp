#include "DirectXFrameBuffer.h"

namespace Hydro
{
    void DirectXFrameBuffer::Destroy()
    {
    }

    void DirectXFrameBuffer::Bind()
    {
    }

    void DirectXFrameBuffer::Unbind()
    {
    }

    AttachmentTexture DirectXFrameBuffer::CreateAttachment(FrameBufferAttachment Attachment)
    {
        return {};
    }

    void DirectXFrameBuffer::RemoveAttachment(const AttachmentTexture& Attachment)
    {
    }

    bool DirectXFrameBuffer::Validate()
    {
        return true;
    }

    void DirectXFrameBuffer::Resize(const Vector2& NewSize)
    {
    }

}