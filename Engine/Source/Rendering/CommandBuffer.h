#pragma once
#include "Filter.h"
#include "Containers/Array.h"
#include "Runtime/Flags.h"
#include "Runtime/Format.h"
#include "ShaderStage.h"
#include "Containers/StringView.h"
#include "Runtime/Ref.h"

namespace Nova { struct Color; }

namespace Nova::Rendering
{
    class Device;
    class CommandPool;
    class Buffer;
    class GraphicsPipeline;
    class ComputePipeline;
    class RenderPass;
    class Texture;
    struct BlitRegion;
    class ShaderBindingSetLayout;
    class ShaderBindingSet;
    class Shader;

    enum class CommandBufferLevel
    {
        Primary,
        Secondary,
    };

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
        CommandBufferUsageFlags flags;
    };

    class CommandBuffer
    {
    public:
        CommandBuffer() = default;
        virtual ~CommandBuffer() = default;

        virtual bool Allocate(const CommandBufferAllocateInfo& allocateInfo) = 0;
        virtual void Free() = 0;
        virtual void SetName(StringView name) = 0;

        virtual bool Begin(const CommandBufferBeginInfo& beginInfo) = 0;
        virtual void End() = 0;

        // Graphics Commands
        virtual void ClearColor(const Color& color, uint32_t attachmentIndex) = 0;
        virtual void ClearDepth(float depth, uint32_t stencil) = 0;
        virtual void BindGraphicsPipeline(const GraphicsPipeline& pipeline) = 0;
        virtual void BindComputePipeline(const ComputePipeline& pipeline) = 0;
        virtual void BindVertexBuffer(const Buffer& vertexBuffer, size_t offset) = 0;
        virtual void BindIndexBuffer(const Buffer& indexBuffer, size_t offset, Format indexFormat) = 0;
        virtual void BindShaderBindingSet(const Shader& shader, const ShaderBindingSet& bindingSet) = 0;
        virtual void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) = 0;
        virtual void SetScissor(int32_t x, int32_t y, int32_t width, int32_t height) = 0;
        virtual void DrawIndexed(size_t count, size_t offset) = 0;
        virtual void BeginRenderPass(const RenderPass& renderPass) = 0;
        virtual void EndRenderPass() = 0;
        virtual void PushConstants(const Ref<Shader>& shader, ShaderStageFlags stageFlags, size_t offset, size_t size, const void* values) = 0;
        virtual void UpdateBuffer(const Buffer& buffer, size_t offset, size_t size, const void* data) = 0;

        // Compute Commands
        virtual void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;
        virtual void DispatchIndirect(const Buffer& buffer, size_t offset) = 0;

        // Transfer Commands
        virtual void BufferCopy(const Buffer& src, const Buffer& dest, size_t srcOffset, size_t destOffset, size_t size) = 0;
        virtual void Blit(const Texture& src, const BlitRegion& srcRegion, const Texture& dest, const BlitRegion& destRegion, Filter filter = Filter::Linear) = 0;
        virtual void Blit(const Texture& src, const Texture& dest, Filter filter = Filter::Linear) = 0;

        virtual void ExecuteCommandBuffers(const Array<CommandBuffer*>& commandBuffers) = 0;

        CommandPool* GetCommandPool() const { return m_CommandPool; }
        CommandBufferLevel GetLevel() const { return m_Level; }
    protected:
        CommandPool* m_CommandPool = nullptr;
        CommandBufferLevel m_Level = CommandBufferLevel::Primary;
    };
}
