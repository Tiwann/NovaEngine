#pragma once

namespace Nova
{
    enum class ResourceState
    {
        Undefined,
        General,
        ShaderRead,
        ShaderWrite,
        ColorAttachment,
        DepthStencilAttachment,
        TransferSource,
        TransferDest,
        Present,
    };
}
