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

    FrameBuffer* FrameBuffer::Create()
    {
        NOVA_RHI_PLATFORM_RETURN(FrameBuffer);
    }
}
