#include "CommandBuffer.h"
#include "Texture.h"
#include "GraphicsPipeline.h"
#include "Buffer.h"

#define COMMAND_BUFFER_CHECK() NOVA_ASSERT(m_State == CommandBufferState::Opened, "Command buffer should be opened!")

namespace Nova::OpenGL
{
    bool CommandBuffer::Allocate(const CommandBufferAllocateInfo& allocateInfo)
    {
        m_State = CommandBufferState::Closed;
        m_Commands.Clear();
        return true;
    }

    void CommandBuffer::Free()
    {
        m_Commands.Free();
        m_State = CommandBufferState::Invalid;
    }

    void CommandBuffer::SetName(const StringView name)
    {
        m_Name = String(name);
    }

    bool CommandBuffer::Begin(const CommandBufferBeginInfo& beginInfo)
    {
        if (m_State != CommandBufferState::Closed)
            return false;
        m_State = CommandBufferState::Opened;
        return true;
    }

    void CommandBuffer::End()
    {
        NOVA_ASSERT(m_State == CommandBufferState::Opened, "Command buffer should be opened!");
        m_State = CommandBufferState::Pending;
    }

    void CommandBuffer::ClearColor(const Color& color, const uint32_t attachmentIndex)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::ClearColor};
        command.data.clearColor = {color, attachmentIndex};
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::ClearDepthStencil(const float depth, const uint32_t stencil)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::ClearDepthStencil};
        command.data.clearDepthStencil = {depth, stencil};
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::BindGraphicsPipeline(const Nova::GraphicsPipeline& pipeline)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::BindGraphicsPipeline};
        command.data.bindGraphicsPipeline.pipeline = static_cast<const GraphicsPipeline*>(&pipeline);
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::BindComputePipeline(const Nova::ComputePipeline& pipeline)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::BindComputePipeline};
        //command.data.bindComputePipeline.pipeline = static_cast<const ComputePipeline*>(&pipeline);
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::BindVertexBuffer(const Nova::Buffer& vertexBuffer, size_t offset)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::BindVertexBuffer};
        command.data.bindVertexBuffer.buffer = static_cast<const Buffer*>(&vertexBuffer);
        command.data.bindVertexBuffer.offset = offset;
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::BindIndexBuffer(const Nova::Buffer& indexBuffer, size_t offset, Format indexFormat)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::BindIndexBuffer};
        command.data.bindIndexBuffer.buffer = static_cast<const Buffer*>(&indexBuffer);
        command.data.bindIndexBuffer.offset = offset;
        command.data.bindIndexBuffer.indexFormat = indexFormat;
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::BindShaderBindingSet(const Nova::Shader& shader, const Nova::ShaderBindingSet& bindingSet)
    {
    }

    void CommandBuffer::BindMaterial(const Nova::Material& material)
    {
    }

    void CommandBuffer::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::SetViewport};
        command.data.setViewport = {x, y, width, height, minDepth, maxDepth};
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::SetScissor(int32_t x, int32_t y, int32_t width, int32_t height)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::SetScissor};
        command.data.setScissor = {x, y, width, height};
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::Draw};
        command.data.draw.vertexCount = vertexCount;
        command.data.draw.instanceCount = instanceCount;
        command.data.draw.firstVertex = firstVertex;
        command.data.draw.firstInstance = firstInstance;
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::DrawIndexed(const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex, const int32_t vertexOffset, const uint32_t firstInstance)
    {
        COMMAND_BUFFER_CHECK();
        Command command {CommandType::DrawIndexed};
        DrawIndexedCommand& cmd = command.data.drawIndexed;
        cmd.indexCount = indexCount;
        cmd.instanceCount = instanceCount;
        cmd.firstIndex = firstIndex;
        cmd.vertexOffset = vertexOffset;
        cmd.firstInstance = firstInstance;
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::DrawIndirect(const Nova::Buffer& buffer, size_t offset, uint32_t drawCount, size_t stride)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::DrawIndirect};
        command.data.drawIndirect.buffer = static_cast<const Buffer*>(&buffer);
        command.data.drawIndirect.offset = offset;
        command.data.drawIndirect.drawCount = drawCount;
        command.data.drawIndirect.stride = static_cast<uint32_t>(stride);
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::BeginRenderPass(const Nova::RenderPassBeginInfo& renderPassBeginInfo)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::RenderPassBegin};
        command.data.renderPassBegin.info = &renderPassBeginInfo;
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::EndRenderPass()
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::RenderPassEnd};
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::PushConstants(const Nova::Shader& shader, ShaderStageFlags stageFlags, size_t offset, size_t size, const void* values)
    {
    }

    void CommandBuffer::UpdateBuffer(const Nova::Buffer& buffer, size_t offset, size_t size, const void* data)
    {

    }

    void CommandBuffer::TextureBarrier(const Nova::TextureBarrier& barrier)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::TextureBarrier};
        command.data.textureBarrier.barrier = &barrier;
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::BufferBarrier(const Nova::BufferBarrier& barrier)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::BufferBarrier};
        command.data.bufferBarrier.barrier = &barrier;
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::MemoryBarrier(const Nova::MemoryBarrier& barrier)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::MemoryBarrier};

        command.data.memoryBarrier.barrier = &barrier;
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::Dispatch};
        command.data.dispatch = {groupCountX, groupCountY, groupCountZ};
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::DispatchIndirect(const Nova::Buffer& buffer, size_t offset)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::DispatchIndirect};
        command.data.dispatchIndirect.buffer = static_cast<const Buffer*>(&buffer);
        command.data.dispatchIndirect.offset = offset;
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::BufferCopy(const Nova::Buffer& src, const Nova::Buffer& dest, size_t srcOffset, size_t destOffset, size_t size)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::BufferCopy};
        command.data.bufferCopy = {static_cast<const Buffer*>(&src), static_cast<const Buffer*>(&dest), srcOffset, destOffset, size};
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::Blit(const Nova::Texture& src, const BlitRegion& srcRegion, const Nova::Texture& dest, const BlitRegion& destRegion, Filter filter)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::Blit};
        command.data.blit = {static_cast<const Texture*>(&src), static_cast<const Texture*>(&dest), srcRegion, destRegion, filter};
        m_Commands.Enqueue(command);
    }

    void CommandBuffer::Blit(const Nova::Texture& src, const Nova::Texture& dest, const Filter filter)
    {
        BlitRegion srcRegion;
        srcRegion.x = 0;
        srcRegion.y = 0;
        srcRegion.width = src.GetWidth();
        srcRegion.height = src.GetHeight();
        srcRegion.mipLevel = 0;

        BlitRegion destRegion;
        destRegion.x = 0;
        destRegion.y = 0;
        destRegion.width = dest.GetWidth();
        destRegion.height = dest.GetHeight();
        destRegion.mipLevel = 0;
        Blit(src, srcRegion, dest, destRegion, filter);
    }

    void CommandBuffer::ExecuteCommandBuffers(const Array<Nova::CommandBuffer*>& commandBuffers)
    {
        COMMAND_BUFFER_CHECK();
        Command command{CommandType::ExecuteCommandBuffers};
        command.data.executeCommandBuffers.commandBuffers = commandBuffers.Data();
        command.data.executeCommandBuffers.commandBufferCount = static_cast<uint32_t>(commandBuffers.Size());
        m_Commands.Enqueue(command);
    }
}
