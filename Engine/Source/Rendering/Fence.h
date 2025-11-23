#pragma once
#include "Runtime/Flags.h"

namespace Nova
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
        Device* device = nullptr;
        FenceCreateFlags flags = FenceCreateFlagBits::None;

        FenceCreateInfo& withDevice(Device* inDevice) { device = inDevice; return *this; }
        FenceCreateInfo& withFlags(const FenceCreateFlags inFlags) { flags = inFlags; return *this; }
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