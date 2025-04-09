#include "FrameBuffer.h"
#include "Texture2D.h"
#include "Platform/PlatformFrameBuffer.h"

namespace Nova
{

    AttachmentTexture& FrameBuffer::GetAttachment(Array<AttachmentTexture>::SizeType Index)
    {
        return m_Attachments.GetAt(Index);
    }

    const Array<AttachmentTexture>& FrameBuffer::GetAttachments() const
    {
        return m_Attachments;
    }

    FrameBuffer* FrameBuffer::Create(const GraphicsApi& GraphicsApi)
    {
        switch (GraphicsApi)
        {
        case GraphicsApi::None:
            return nullptr;
        case GraphicsApi::OpenGL:
            return new OpenGLFrameBuffer();
        case GraphicsApi::Vulkan:
            return new VulkanFrameBuffer();
        case GraphicsApi::D3D12:
            return new D3D12FrameBuffer();
        default:
            return nullptr;
        }
    }
}
