﻿#pragma once
#include "Runtime/Flags.h"
#include "Runtime/Object.h"

namespace Nova
{
    enum class CommandBufferLevel
    {
        Primary,
        Secondary,
    };

    struct CommandBufferAllocateInfo
    {
        CommandBufferLevel Level = CommandBufferLevel::Primary;
    };

    enum class CommandBufferUsageFlagBits
    {
        None = 0,
        OneTimeSubmit = BIT(0),
        RenderPassContinue = BIT(1),
        Simultaneous = BIT(2),
    };

    using CommandBufferUsageFlags = Flags<CommandBufferUsageFlagBits>;

    struct CommandBufferBeginInfo
    {
        CommandBufferUsageFlags Flags = CommandBufferUsageFlagBits::None;
    };

    class CommandPool;

    class CommandBuffer : public Object
    {
    public:
        virtual bool Initialize(const CommandBufferAllocateInfo& AllocateInfo) = 0;
        virtual void Destroy() = 0;
        virtual bool Begin(const CommandBufferBeginInfo& BeginInfo) = 0;
        virtual bool End() = 0;
        virtual bool IsValid() const = 0;
        virtual void SetDebugName(const String& Name){}
        CommandPool* GetCommandPool() const;
        CommandBufferLevel GetLevel() const;
    protected:
        friend CommandPool;
        explicit CommandBuffer(CommandPool* Owner);
        CommandPool* m_CommandPool = nullptr;
        CommandBufferLevel m_Level = CommandBufferLevel::Primary;
    };
}
