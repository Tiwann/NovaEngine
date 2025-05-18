#pragma once
#include "RendererObject.h"
#include "Runtime/Flags.h"

namespace Nova
{
    enum class FenceCreateFlagBits
    {
        Signaled = BIT(0)
    };

    NOVA_DECLARE_FLAGS(FenceCreateFlagBits, FenceCreateFlags);

    struct FenceCreateInfo
    {
        FenceCreateFlags Flags;
    };

    class Renderer;

    class Fence : public RendererObject<FenceCreateInfo>
    {
    public:
        explicit Fence(Renderer* Owner) : RendererObject("Fence", Owner){}
        virtual void Reset() = 0;
    };
}