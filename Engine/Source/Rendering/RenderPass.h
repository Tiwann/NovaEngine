#pragma once
#include "LoadOperation.h"
#include "StoreOperation.h"
#include "ResolveMode.h"
#include "Runtime/Color.h"
#include "Containers/BufferView.h"
#include "Math/Rect.h"

#include <cstdint>


namespace Nova
{
    class Texture;

    enum class RenderPassAttachmentType
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

    struct RenderPassAttachmentInfo
    {
        RenderPassAttachmentType type = RenderPassAttachmentType::Color;
        ClearValue clearValue = ClearValue();
        LoadOperation loadOp = LoadOperation::DontCare;
        StoreOperation storeOp = StoreOperation::DontCare;
        ResolveMode resolveMode = ResolveMode::None;
        const Texture* texture = nullptr;
        const Texture* resolveTexture = nullptr;
    };

    struct RenderPassBeginInfo
    {
        RenderPassAttachmentInfo* colorAttachments = nullptr;
        uint32_t colorAttachmentCount = 0;
        RenderPassAttachmentInfo* depthAttachment = nullptr;
        Rect2D<uint32_t> renderArea;
    };
}
