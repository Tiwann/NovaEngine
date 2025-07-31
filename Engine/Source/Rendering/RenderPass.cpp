#include "RenderPass.h"

namespace Nova::Rendering
{
    RenderPass::RenderPass(const uint32_t offsetX, const uint32_t offsetY, const uint32_t width, const uint32_t height) :
        m_OffsetX(offsetX), m_OffsetY(offsetY), m_Width(width), m_Height(height)
    {
    }

    void RenderPass::AddAttachment(const RenderPassAttachment& attachment)
    {
        if (GetDepthAttachmentCount() > 0 && attachment.type == AttachmentType::Depth)
            NOVA_ASSERT(false, "Only one depth attachment is allowed");
        m_Attachments.Add(attachment);
    }

    uint32_t RenderPass::GetOffsetX() const
    {
        return m_OffsetX;
    }

    uint32_t RenderPass::GetOffsetY() const
    {
        return m_OffsetY;
    }

    uint32_t RenderPass::GetWidth() const
    {
        return m_Width;
    }

    uint32_t RenderPass::GetHeight() const
    {
        return m_Height;
    }

    uint32_t RenderPass::GetAttachmentCount() const
    {
        return m_Attachments.Count();
    }

    uint32_t RenderPass::GetColorAttachmentCount() const
    {
        uint32_t result = 0;
        for (const RenderPassAttachment& attachment : m_Attachments)
        {
            if (attachment.type == AttachmentType::Color)
                result++;
        }
        return result;
    }

    uint32_t RenderPass::GetDepthAttachmentCount() const
    {
        uint32_t result = 0;
        for (const RenderPassAttachment& attachment : m_Attachments)
        {
            if (attachment.type == AttachmentType::Depth)
                result++;
        }
        return result;
    }

    RenderPassAttachment* RenderPass::GetDepthAttachment() const
    {
        return m_Attachments.Single([](const RenderPassAttachment& attachment)
        {
            return attachment.type == AttachmentType::Depth;
        });
    }

    bool RenderPass::HasDepthAttachment() const
    {
        return GetDepthAttachmentCount() > 0;
    }

    bool RenderPass::HasStencilAttachment() const
    {
        return false;
    }

    Array<RenderPassAttachment>::Iterator RenderPass::begin()
    {
        return m_Attachments.begin();
    }

    Array<RenderPassAttachment>::Iterator RenderPass::end()
    {
        return m_Attachments.end();
    }

    Array<RenderPassAttachment>::ConstIterator RenderPass::begin() const
    {
        return m_Attachments.begin();
    }

    Array<RenderPassAttachment>::ConstIterator RenderPass::end() const
    {
        return m_Attachments.end();
    }
}
