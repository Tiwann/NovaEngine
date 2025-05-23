﻿#pragma once
#include "RendererObject.h"
#include "Runtime/Flags.h"

namespace Nova
{
    class Renderer;
    class CommandBuffer;
    struct CommandBufferAllocateInfo;

    enum class CommandPoolFlagBits
    {
        None = 0,
        Transient = BIT(0),
        ResetCommandBuffer = BIT(1),
        Protected = BIT(2),
    };

    using CommandPoolFlags = Flags<CommandPoolFlagBits>;

    struct CommandPoolCreateInfo
    {
        CommandPoolFlags Flags = CommandPoolFlagBits::None;
    };

    class CommandPool : public RendererObject<CommandPoolCreateInfo>
    {
    public:
        explicit CommandPool(Renderer* Owner) : RendererObject("Command Pool", Owner) {}

        CommandBuffer* AllocateCommandBuffer(const CommandBufferAllocateInfo& AllocateInfo);
        void FreeCommandBuffer(CommandBuffer* CommandBuffer);

    protected:
        CommandPoolFlags m_Flags;
    };
}
