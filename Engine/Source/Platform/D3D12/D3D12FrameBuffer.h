#pragma once
#include "Rendering/FrameBuffer.h"

namespace Nova
{
    class D3D12FrameBuffer : public FrameBuffer
    {
    public:
        D3D12FrameBuffer() = default;
        void Destroy() override;
        void Bind() override;
        void Unbind() override;
        AttachmentTexture CreateAttachment(FrameBufferAttachment Attachment) override;
        void RemoveAttachment(const AttachmentTexture& Attachment) override;
        bool Validate() override;
        void Resize(const Vector2& NewSize) override;
    };
}
