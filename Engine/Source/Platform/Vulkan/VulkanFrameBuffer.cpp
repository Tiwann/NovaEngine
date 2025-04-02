#include "VulkanFrameBuffer.h"

void Hydro::VulkanFrameBuffer::Destroy()
{
}

void Hydro::VulkanFrameBuffer::Bind()
{
}

void Hydro::VulkanFrameBuffer::Unbind()
{
}

Hydro::AttachmentTexture Hydro::VulkanFrameBuffer::CreateAttachment(FrameBufferAttachment Attachment)
{
    return {  .Attachment= FrameBufferAttachment::None, .Texture= nullptr };
}

void Hydro::VulkanFrameBuffer::RemoveAttachment(const AttachmentTexture& Attachment)
{
}

bool Hydro::VulkanFrameBuffer::Validate()
{
    return true;
}

void Hydro::VulkanFrameBuffer::Resize(const Vector2& NewSize)
{
}
