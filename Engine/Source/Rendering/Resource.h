#pragma once
#include "Runtime/RefObject.h"

#define NOVA_CONCAT_IMPL(a,b) a##b
#define NOVA_CONCAT(a, b) NOVA_CONCAT_IMPL(a,b)
#define NOVA_DEFER_REF(x, memberFunc) Nova::Defer NOVA_CONCAT(defer_, __LINE__)(x.Get(), memberFunc)
#define NOVA_DEFER(x, memberFunc) Nova::Defer NOVA_CONCAT(defer_, __LINE__)(x, memberFunc)
#define NOVA_DEFER_FUNC(x) Nova::Defer NOVA_CONCAT(defer_, __LINE__)(x)

namespace Nova
{
    enum class ResourceType
    {
        Texture,
        Sampler,
        Buffer,
    };

    class Resource : public RefObject
    {
    public:
        Resource() = default;
        Resource(const Resource&) = delete;
        Resource(const Resource&&) = delete;
        Resource& operator=(const Resource&) = delete;
        Resource& operator=(const Resource&&) = delete;

        virtual ResourceType GetResourceType() = 0;
    };
}
