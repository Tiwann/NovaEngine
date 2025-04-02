#pragma once
#include "Core/Graphics/FrameBuffer.h"

namespace Hydro
{
    class DirectXFrameBuffer : public FrameBuffer
    {
    public:
        DirectXFrameBuffer() = default;
        void Destroy() override;
        void Bind() override;
        void Unbind() override;
        AttachmentTexture CreateAttachment(FrameBufferAttachment Attachment) override;
        void RemoveAttachment(const AttachmentTexture& Attachment) override;
        bool Validate() override;
        void Resize(const Vector2& NewSize) override;
    };
}
