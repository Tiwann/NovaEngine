#include "VulkanFrameBuffer.h"

void Nova::VulkanFrameBuffer::Destroy()
{
}

void Nova::VulkanFrameBuffer::Bind()
{
}

void Nova::VulkanFrameBuffer::Unbind()
{
}

Nova::AttachmentTexture Nova::VulkanFrameBuffer::CreateAttachment(FrameBufferAttachment Attachment)
{
    return {  .Attachment= FrameBufferAttachment::None, .Texture= nullptr };
}

void Nova::VulkanFrameBuffer::RemoveAttachment(const AttachmentTexture& Attachment)
{
}

bool Nova::VulkanFrameBuffer::Validate()
{
    return true;
}

void Nova::VulkanFrameBuffer::Resize(const Vector2& NewSize)
{
}
