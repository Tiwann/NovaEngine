#pragma once
#include "Runtime/Object.h"
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


    class CommandPool : public Object
    {
    public:
        explicit CommandPool(Renderer* Owner, const CommandPoolCreateInfo& CreateInfo);
        ~CommandPool() override = default;

        virtual CommandBuffer* AllocateCommandBuffer(const CommandBufferAllocateInfo& AllocateInfo) = 0;
        virtual void FreeCommandBuffer(CommandBuffer* CommandBuffer) = 0;

        Renderer* GetRenderer() const;
    protected:
        Renderer* m_Renderer;
        Array<CommandBuffer*> m_AllocatedCommandBuffers;
    private:
        CommandPoolFlags m_Flags;
    };
}
