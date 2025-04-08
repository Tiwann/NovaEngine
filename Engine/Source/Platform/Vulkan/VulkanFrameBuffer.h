#pragma once
#include "Graphics/FrameBuffer.h"

namespace Nova
{
    class VulkanFrameBuffer : public FrameBuffer
    {
    public:
        VulkanFrameBuffer() = default;
        void Destroy() override;
        void Bind() override;
        void Unbind() override;
        AttachmentTexture CreateAttachment(FrameBufferAttachment Attachment) override;
        void RemoveAttachment(const AttachmentTexture& Attachment) override;
        bool Validate() override;
        void Resize(const Vector2& NewSize) override;

    private:
    };
}
