#pragma once
#include "LoadOperation.h"
#include "Runtime/Color.h"
#include "StoreOperation.h"
#include "Containers/Array.h"

#include <cstdint>


namespace Nova::Rendering
{
    class Texture;
    class RenderTarget;

    struct RenderPassCreateInfo
    {
        uint32_t x = 0;
        uint32_t y = 0;
        uint32_t width = 0;
        uint32_t height = 0;
    };

    enum class AttachmentType
    {
        Color,
        Depth,
    };

    struct ClearValue
    {
        Color color = Color::Black;
        float depth = 1.0f;
        uint32_t stencil = 0;
    };

    struct RenderPassAttachment
    {
        AttachmentType type = AttachmentType::Color;
        ClearValue clearValue = ClearValue();
        LoadOperation loadOp = LoadOperation::DontCare;
        StoreOperation storeOp = StoreOperation::DontCare;
        const Texture* texture = nullptr;
        const Texture* resolveTexture = nullptr;
    };

    class RenderPass
    {
    public:
        RenderPass(uint32_t offsetX, uint32_t offsetY, uint32_t width, uint32_t height);

        void AddAttachment(RenderPassAttachment& attachment);

        uint32_t GetOffsetX() const;
        uint32_t GetOffsetY() const;
        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        uint32_t GetAttachmentCount() const;
        uint32_t GetColorAttachmentCount() const;
        uint32_t GetDepthAttachmentCount() const;
        RenderPassAttachment* GetDepthAttachment() const;

        void SetAttachmentTexture(size_t attachmentIndex, const Texture& texture);
        void SetAttachmentResolveTexture(size_t attachmentIndex, const Texture& resolveTexture);

        bool HasDepthAttachment() const;
        bool HasStencilAttachment() const;

        Array<RenderPassAttachment*>::Iterator begin();
        Array<RenderPassAttachment*>::Iterator end();
        Array<RenderPassAttachment*>::ConstIterator begin() const;
        Array<RenderPassAttachment*>::ConstIterator end() const;

    private:
        Array<RenderPassAttachment*> m_Attachments;
        uint32_t m_OffsetX = 0;
        uint32_t m_OffsetY = 0;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
    };
}
