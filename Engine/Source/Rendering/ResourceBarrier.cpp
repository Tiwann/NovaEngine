#include "ResourceBarrier.h"

namespace Nova
{
    ResourceAccessFlags GetSourceAccessFlags(const ResourceState resourceState)
    {
        switch (resourceState)
        {
        case ResourceState::Undefined: return ResourceAccessFlagBits::None;
        case ResourceState::ColorAttachment: return ResourceAccessFlagBits::ColorAttachmentWrite;
        case ResourceState::DepthStencilAttachment: return ResourceAccessFlagBits::DepthStencilAttachmentWrite;
        case ResourceState::TransferSource: return ResourceAccessFlagBits::TransferRead;
        case ResourceState::TransferDest: return ResourceAccessFlagBits::TransferWrite;
        case ResourceState::ShaderRead: return ResourceAccessFlagBits::ShaderRead;
        case ResourceState::ShaderWrite: return ResourceAccessFlagBits::ShaderWrite;
        case ResourceState::General: return ResourceAccessFlagBits::None;
        default: return ResourceAccessFlagBits::None;
        }
    }

    ResourceAccessFlags GetDestAccessFlags(const ResourceState resourceState)
    {
        switch (resourceState)
        {
        case ResourceState::Undefined: return ResourceAccessFlagBits::None;
        case ResourceState::ColorAttachment: return ResourceAccessFlagBits::ColorAttachmentRead;
        case ResourceState::DepthStencilAttachment: return ResourceAccessFlagBits::DepthStencilAttachmentRead;
        case ResourceState::TransferSource: return ResourceAccessFlagBits::TransferRead;
        case ResourceState::TransferDest: return ResourceAccessFlagBits::TransferWrite;
        case ResourceState::ShaderRead: return ResourceAccessFlagBits::ShaderRead;
        case ResourceState::ShaderWrite: return ResourceAccessFlagBits::ShaderWrite;
        case ResourceState::General: return ResourceAccessFlagBits::None;
        default: return ResourceAccessFlagBits::None;
        }
    }
}
