#pragma once
#include "Runtime/Object.h"
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

    class Fence : public Object
    {
    public:
        explicit Fence(Renderer* Owner) : Object("Fence"), m_Owner(Owner) {}

        virtual bool Initialize(const FenceCreateInfo& CreateInfo) = 0;
        virtual void Destroy() = 0;

        Renderer* GetOwner() const { return m_Owner; }
    protected:
        Renderer* m_Owner = nullptr;
    };
}