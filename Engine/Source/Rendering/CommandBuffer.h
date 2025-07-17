#pragma once
#include "Runtime/Flags.h"
#include "Runtime/Format.h"

namespace Nova { struct Color; }
namespace Nova::Rendering
{
    enum class CommandBufferLevel
    {
        Primary,
        Secondary,
    };

    class Device;
    class CommandPool;
    class Buffer;

    struct CommandBufferAllocateInfo
    {
        Device* device;
        CommandPool* commandPool;
        CommandBufferLevel level = CommandBufferLevel::Primary;
    };

    enum class CommandBufferUsageFlagBits
    {
        None = 0,
        OneTimeSubmit = BIT(0),
        RenderPassContinue = BIT(1),
        Simultaneous = BIT(2),
    };

    typedef Flags<CommandBufferUsageFlagBits> CommandBufferUsageFlags;

    struct CommandBufferBeginInfo
    {
        CommandBufferUsageFlags Flags;
    };

    class CommandBuffer
    {
    public:
        CommandBuffer() = default;
        virtual ~CommandBuffer() = default;

        virtual bool Allocate(const CommandBufferAllocateInfo& allocateInfo) = 0;
        virtual void Free() = 0;

        virtual bool Begin(const CommandBufferBeginInfo& beginInfo) = 0;
        virtual void End() = 0;


        virtual void ClearColor(const Color& color) = 0;
        virtual void ClearDepth(float depth, uint8_t stencil) = 0;
        virtual void Clear(const Color& color, float depth, uint8_t stencil) { ClearColor(color); ClearDepth(depth, stencil); }
        virtual void BindVertexBuffer(const Buffer& vertexBuffer, size_t offset) = 0;
        virtual void BindIndexBuffer(const Buffer& indexBuffer, size_t offset, Format indexFormat) = 0;

        CommandPool* GetCommandPool() const { return m_CommandPool; }
        CommandBufferLevel GetLevel() const { return m_Level; }
    protected:
        CommandPool* m_CommandPool = nullptr;
        CommandBufferLevel m_Level = CommandBufferLevel::Primary;
    };
}
