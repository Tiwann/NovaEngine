#pragma once
#include "Runtime/Flags.h"

namespace Nova::Rendering
{
    enum class FenceCreateFlagBits
    {
        None = 0,
        Signaled = BIT(0)
    };

    typedef Flags<FenceCreateFlagBits> FenceCreateFlags;


    class Device;

    struct FenceCreateInfo
    {
        Device* device;
        FenceCreateFlags flags;
    };

    class Fence
    {
    public:
        Fence() = default;
        virtual ~Fence() = default;

        virtual bool Initialize(const FenceCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;
        virtual void Wait(uint64_t timeout) = 0;
        virtual void Reset() = 0;
    };
}