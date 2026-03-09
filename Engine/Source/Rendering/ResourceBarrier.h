#pragma once
#include "ResourceState.h"
#include "Runtime/Flags.h"

namespace Nova
{
    class Texture;
    class Buffer;
    class Queue;

    enum class AccessFlagBits
    {
        None = 0,
        ShaderRead = BIT(0),
        ShaderWrite = BIT(1),
        ColorAttachmentRead = BIT(2),
        ColorAttachmentWrite = BIT(3),
        DepthStencilAttachmentRead = BIT(4),
        DepthStencilAttachmentWrite = BIT(5),
        TransferRead = BIT(6),
        TransferWrite = BIT(7),
        HostRead = BIT(8),
        HostWrite = BIT(9),
    };

    NOVA_DECLARE_FLAGS(AccessFlagBits, AccessFlags);

    struct TextureBarrier
    {
        Texture* texture = nullptr;
        ResourceState destState = ResourceState::Undefined;
        AccessFlags sourceAccess = AccessFlagBits::None;
        AccessFlags destAccess = AccessFlagBits::None;
        const Queue* sourceQueue = nullptr;
        const Queue* destQueue = nullptr;
    };

    struct BufferBarrier
    {
        Buffer* buffer = nullptr;
        uint64_t offset = 0;
        uint64_t size = 0;
        AccessFlags sourceAccess = AccessFlagBits::None;
        AccessFlags destAccess = AccessFlagBits::None;
        const Queue* sourceQueue = nullptr;
        const Queue* destQueue = nullptr;
    };

    struct MemoryBarrier
    {
        uint32_t textureBarrierCount = 0;
        TextureBarrier* textureBarriers = nullptr;
        uint32_t bufferBarrierCount = 0;
        BufferBarrier* bufferBarriers = nullptr;
    };

    static AccessFlags GetSourceAccessFlags(const ResourceState resourceState)
    {
        switch (resourceState)
        {
        case ResourceState::Undefined: return AccessFlagBits::None;
        case ResourceState::ColorAttachment: return AccessFlagBits::ColorAttachmentWrite;
        case ResourceState::DepthStencilAttachment: return AccessFlagBits::DepthStencilAttachmentWrite;
        case ResourceState::TransferSource: return AccessFlagBits::TransferRead;
        case ResourceState::TransferDest: return AccessFlagBits::TransferWrite;
        case ResourceState::ShaderRead: return AccessFlagBits::ShaderRead;
        case ResourceState::ShaderWrite: return AccessFlagBits::ShaderWrite;
        case ResourceState::General: return AccessFlagBits::None;
        default: return AccessFlagBits::None;
        }
    }

    static AccessFlags GetDestAccessFlags(const ResourceState resourceState)
    {
        switch (resourceState)
        {
        case ResourceState::Undefined: return AccessFlagBits::None;
        case ResourceState::ColorAttachment: return AccessFlagBits::ColorAttachmentRead;
        case ResourceState::DepthStencilAttachment: return AccessFlagBits::DepthStencilAttachmentRead;
        case ResourceState::TransferSource: return AccessFlagBits::TransferWrite;
        case ResourceState::TransferDest: return AccessFlagBits::TransferRead;
        case ResourceState::ShaderRead: return AccessFlagBits::ShaderRead;
        case ResourceState::ShaderWrite: return AccessFlagBits::ShaderWrite;
        case ResourceState::General: return AccessFlagBits::None;
        default: return AccessFlagBits::None;
        }
    }
}
