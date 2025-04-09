#include "D3D12FrameBuffer.h"

namespace Nova
{
    void D3D12FrameBuffer::Destroy()
    {
    }

    void D3D12FrameBuffer::Bind()
    {
    }

    void D3D12FrameBuffer::Unbind()
    {
    }

    AttachmentTexture D3D12FrameBuffer::CreateAttachment(FrameBufferAttachment Attachment)
    {
        return {};
    }

    void D3D12FrameBuffer::RemoveAttachment(const AttachmentTexture& Attachment)
    {
    }

    bool D3D12FrameBuffer::Validate()
    {
        return true;
    }

    void D3D12FrameBuffer::Resize(const Vector2& NewSize)
    {
    }

}