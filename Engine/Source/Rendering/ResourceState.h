#pragma once
#include "Runtime/Object.h"

namespace Nova
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

    class Resource : public Object
    {
    public:
        ~Resource() override = default;
    };
}
