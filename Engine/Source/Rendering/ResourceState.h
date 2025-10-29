#pragma once

namespace Nova::Rendering
{
    enum class ResourceState
    {
        Unknown,
        General,
        ShaderResourceView,
        UnorderedAccessView,
        CopyDest,
        CopySource,
        ResolveDest,
        ResolveSource,
        Present,
    };
}
