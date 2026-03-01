#pragma once
#include "Filter.h"
#include "Containers/Array.h"
#include "Runtime/Flags.h"
#include "Runtime/Format.h"
#include "ShaderStage.h"
#include "Containers/StringView.h"


namespace Nova
{
    class RenderDevice;
    class CommandPool;
    class Buffer;
    class GraphicsPipeline;
    class ComputePipeline;
    class Texture;
    struct BlitRegion;
    class ShaderBindingSetLayout;
    class ShaderBindingSet;
    class Shader;
    class Material;
    class StaticMesh;
    struct Color;
    class Camera;
    struct TextureBarrier;
    struct BufferBarrier;
    struct MemoryBarrier;
    struct RenderPassBeginInfo;

    struct DrawIndirectParameters
    {
        uint32_t    vertexCount;
        uint32_t    instanceCount;
        uint32_t    firstVertex;
        uint32_t    firstInstance;
    };

    enum class CommandBufferLevel
    {
        Primary,
        Secondary,
    };

    struct CommandBufferAllocateInfo
    {
        RenderDevice* device;
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
        virtual void ClearDepthStencil(float depth, uint32_t stencil) = 0;
        virtual void BindGraphicsPipeline(const Nova::GraphicsPipeline& pipeline) = 0;
        virtual void BindComputePipeline(const Nova::ComputePipeline& pipeline) = 0;
        virtual void BindVertexBuffer(const Nova::Buffer& vertexBuffer, size_t offset) = 0;
        virtual void BindIndexBuffer(const Nova::Buffer& indexBuffer, size_t offset, Format indexFormat) = 0;
        virtual void BindShaderBindingSet(const Nova::Shader& shader, const Nova::ShaderBindingSet& bindingSet) = 0;
        virtual void BindMaterial(const Nova::Material& material) = 0;
        virtual void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) = 0;
        virtual void SetScissor(int32_t x, int32_t y, int32_t width, int32_t height) = 0;
        virtual void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;
        virtual void DrawIndirect(const Nova::Buffer& buffer, size_t offset, uint32_t drawCount, size_t stride = sizeof(DrawIndirectParameters)) = 0;
        virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) = 0;
        virtual void BeginRenderPass(const Nova::RenderPassBeginInfo& renderPass) = 0;
        virtual void EndRenderPass() = 0;
        virtual void PushConstants(const Nova::Shader& shader, ShaderStageFlags stageFlags, size_t offset, size_t size, const void* values) = 0;
        virtual void UpdateBuffer(const Nova::Buffer& buffer, size_t offset, size_t size, const void* data) = 0;
        virtual void TextureBarrier(const TextureBarrier& barrier) = 0;
        virtual void BufferBarrier(const BufferBarrier& barrier) = 0;
        virtual void MemoryBarrier(const MemoryBarrier& barrier) = 0;

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
