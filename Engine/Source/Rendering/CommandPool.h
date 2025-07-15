#pragma once
#include "Runtime/Flags.h"

namespace Nova::Rendering
{
    enum class CommandPoolCreateFlagBits
    {
        None = 0,
        Transient = BIT(0),
        ResetCommandBuffer = BIT(1),
        Protected = BIT(2),
    };

    typedef Flags<CommandPoolCreateFlagBits> CommandPoolCreateFlags;

    class Device;

    struct CommandPoolCreateInfo
    {
        Device* device;
        CommandPoolCreateFlags flags;
    };

    class CommandBuffer;
    struct CommandBufferAllocateInfo;

    class CommandPool
    {
    public:
        CommandPool() = default;
        virtual ~CommandPool() = default;

        virtual bool Initialize(const CommandPoolCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;
    protected:
        Device* m_Device = nullptr;
    };
}
