#pragma once
#include "Runtime/Color.h"
#include "Runtime/Format.h"
#include "Rendering/BlitRegion.h"
#include "Rendering/Filter.h"

namespace Nova::OpenGL
{
    class GraphicsPipeline;
    class ComputePipeline;
    class Buffer;
    class Texture;

    enum class CommandType
    {
        ClearColor,
        ClearDepthStencil,
        BindGraphicsPipeline,
        BindComputePipeline,
        BindVertexBuffer,
        BindIndexBuffer,
        SetViewport,
        SetScissor,
        Draw,
        DrawIndexed,
        DrawIndirect,
        DrawIndexedIndirect,
        Dispatch,
        DispatchIndirect,
        TextureBarrier,
        BufferBarrier,
        MemoryBarrier,
        BufferCopy,
        Blit,
        RenderPassBegin,
        RenderPassEnd,
        ExecuteCommandBuffers,
    };

    struct ClearColorCommand
    {
        Color color = Color::Black;
        uint32_t attachmentIndex = 0;
    };

    struct ClearDepthStencilCommand
    {
        float depth = 1.0f;
        uint32_t stencil = 0;
    };

    struct BindGraphicsPipelineCommand
    {
        const class GraphicsPipeline* pipeline = nullptr;
    };

    struct BindComputePipelineCommand
    {
        const class ComputePipeline* pipeline = nullptr;
    };

    struct BindVertexBufferCommand
    {
        const class Buffer* buffer = nullptr;
        uint32_t binding = 0;
        uint64_t offset = 0;
    };

    struct BindIndexBufferCommand
    {
        const Buffer* buffer = nullptr;
        uint64_t offset = 0;
        Format indexFormat = Format::None;
    };

    struct SetViewportCommand
    {
        float x = 0.0f;
        float y = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
        float minDepth = 0.0f;
        float maxDepth = 0.0f;
    };

    struct SetScissorCommand
    {
        int32_t x = 0.0f;
        int32_t y = 0.0f;
        int32_t width = 0.0f;
        int32_t height = 0.0f;
    };

    struct DrawCommand
    {
        uint32_t vertexCount = 0;
        uint32_t instanceCount = 0;
        uint32_t firstVertex = 0;
        uint32_t firstInstance = 0;
    };

    struct DrawIndexedCommand
    {
        uint32_t indexCount = 0;
        uint32_t instanceCount = 0;
        uint32_t firstIndex = 0;
        int32_t vertexOffset = 0;
        uint32_t firstInstance = 0;
    };

    struct DrawIndirectCommand
    {
        const Buffer* buffer = nullptr;
        uint64_t offset = 0;
        uint32_t drawCount = 0;
        uint32_t stride = 0;
    };

    struct DrawIndexedIndirectCommand
    {
        const Buffer* buffer = nullptr;
        uint64_t offset = 0;
        uint32_t drawCount = 0;
        uint32_t stride = 0;
    };

    struct DispatchCommand
    {
        uint32_t groupCountX = 0;
        uint32_t groupCountY = 0;
        uint32_t groupCountZ = 0;
    };

    struct DispatchIndirectCommand
    {
        const Buffer* buffer = nullptr;
        uint64_t offset = 0;
    };

    struct TextureBarrierCommand
    {
        const struct TextureBarrier* barrier = nullptr;
    };

    struct BufferBarrierCommand
    {
        const struct BufferBarrier* barrier = nullptr;
    };

    struct MemoryBarrierCommand
    {
        const struct MemoryBarrier* barrier = nullptr;
    };

    struct BufferCopyCommand
    {
        const Buffer* srcBuffer = nullptr;
        const Buffer* dstBuffer = nullptr;
        size_t srcOffset = 0;
        size_t dstOffset = 0;
        size_t size = 0;
    };

    struct BlitCommand
    {
        const Texture* srcTexture = nullptr;
        const Texture* dstTexture = nullptr;
        BlitRegion srcRegion;
        BlitRegion dstRegion;
        Filter filter;
    };

    struct RenderPassBeginCommand
    {
        const struct RenderPassBeginInfo* info = nullptr;
    };

    struct RenderPassEndCommand
    {
        const struct RenderPassBeginInfo* info = nullptr;
    };

    struct ExecuteCommandBuffersCommand
    {
        class CommandBuffer* const * commandBuffers = nullptr;
        uint32_t commandBufferCount = 0;
    };

    union CommandData
    {
        ClearColorCommand clearColor;
        ClearDepthStencilCommand clearDepthStencil;
        BindGraphicsPipelineCommand bindGraphicsPipeline;
        BindComputePipelineCommand bindComputePipeline;
        BindVertexBufferCommand bindVertexBuffer;
        BindIndexBufferCommand bindIndexBuffer;
        SetViewportCommand setViewport;
        SetScissorCommand setScissor;
        DrawCommand draw;
        DrawIndexedCommand drawIndexed;
        DrawIndirectCommand drawIndirect;
        DrawIndexedIndirectCommand drawIndexedIndirect;
        DispatchCommand dispatch;
        DispatchIndirectCommand dispatchIndirect;
        TextureBarrierCommand textureBarrier;
        BufferBarrierCommand bufferBarrier;
        MemoryBarrierCommand memoryBarrier;
        BufferCopyCommand bufferCopy;
        BlitCommand blit;
        RenderPassBeginCommand renderPassBegin;
        RenderPassBeginCommand renderPassEnd;
        ExecuteCommandBuffersCommand executeCommandBuffers;
    };

    struct Command
    {
        CommandType type;
        CommandData data;
    };
}
