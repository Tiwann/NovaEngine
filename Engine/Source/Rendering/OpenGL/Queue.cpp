#include "Queue.h"
#include "CommandBuffer.h"
#include "Buffer.h"
#include "Texture.h"
#include "GraphicsPipeline.h"
#include "Conversions.h"
#include "Rendering/Vulkan/Conversions.h"

#include <glad/glad.h>

#define CHECK_VALID_GRAPHICS_PIPELINE(pipeline) NOVA_ASSERT(pipeline, "A valid graphics pipeline should be bound before executing this command!");
#define CHECK_VALID_COMPUTE_PIPELINE(pipeline) NOVA_ASSERT(pipeline, "A valid compute pipeline should be bound before executing this command!");
#define CHECK_VALID_INDIRECT_BUFFER(buffer) NOVA_ASSERT((buffer)->GetUsage() == BufferUsage::IndirectBuffer, "You must use a buffer created with BufferUsage::IndirectBuffer!")

namespace Nova::OpenGL
{
    void Queue::Submit(Nova::CommandBuffer* commandBuffer, Semaphore* waitSemaphore, Semaphore* signalSemaphore, Fence* fence, uint32_t waitStagesMask) const
    {
        NOVA_ASSERT(commandBuffer, "Tried to submit an invalid command buffer!");
        CommandBuffer* cmdBuffer = static_cast<CommandBuffer*>(commandBuffer);
        NOVA_ASSERT(cmdBuffer->m_State == CommandBufferState::Pending, "CommandBuffer must be in a pending state for submit!");

        Fifo<Command>& commandsFifo = cmdBuffer->m_Commands;
        const GraphicsPipeline* graphicsPipeline = nullptr;
        const ComputePipeline* computePipeline = nullptr;
        PrimitiveTopology primitiveTopology = PrimitiveTopology::TriangleList;
        size_t vertexBufferStride = 0;
        Format indexFormat = Format::None;
        bool insideRenderPass = false;

        while (!commandsFifo.IsEmpty())
        {
            const Command command = commandsFifo.Dequeue();
            const CommandData& data = command.data;
            switch (command.type)
            {
            case CommandType::ClearColor:
                {
                    const ClearColorCommand& cmd = data.clearColor;
                    glClearBufferfv(GL_COLOR, cmd.attachmentIndex, reinterpret_cast<const GLfloat*>(&cmd.color));
                    break;
                }
            case CommandType::ClearDepthStencil:
                {
                    const ClearDepthStencilCommand& cmd = data.clearDepthStencil;
                    glClearBufferfi(GL_DEPTH_STENCIL, 0, cmd.depth, cmd.stencil);
                    break;
                }
            case CommandType::BindGraphicsPipeline:
                {
                    const BindGraphicsPipelineCommand& cmd = data.bindGraphicsPipeline;
                    cmd.pipeline->Bind();
                    primitiveTopology = cmd.pipeline->m_PipelineDesc.inputAssemblyInfo.topology;
                    vertexBufferStride = cmd.pipeline->m_PipelineDesc.vertexInputInfo.layout.Stride();
                    graphicsPipeline = static_cast<const GraphicsPipeline*>(cmd.pipeline);
                    break;
                }
            case CommandType::BindComputePipeline:
                {
                    const BindComputePipelineCommand& cmd = data.bindComputePipeline;
                    computePipeline = cmd.pipeline;
                    //cmd.pipeline->Bind();
                    break;
                }
            case CommandType::BindVertexBuffer:
                {
                    const BindVertexBufferCommand& cmd = data.bindVertexBuffer;
                    const Buffer* buffer = cmd.buffer;
                    glBindVertexBuffer(cmd.binding, buffer->GetHandle(), cmd.offset, vertexBufferStride);
                    break;
                }
            case CommandType::BindIndexBuffer:
                {
                    const BindIndexBufferCommand& cmd = data.bindIndexBuffer;
                    const Buffer* buffer = cmd.buffer;
                    indexFormat = cmd.indexFormat;
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->GetHandle());
                }
                break;
            case CommandType::SetViewport:
                {
                    const SetViewportCommand& cmd = data.setViewport;
                    glViewport(cmd.x, cmd.y, cmd.width, cmd.height);
                    glDepthRangef(cmd.minDepth, cmd.maxDepth);
                    break;
                }
            case CommandType::SetScissor:
                {
                    const SetScissorCommand& cmd = data.setScissor;
                    glScissor(cmd.x, cmd.y, cmd.width, cmd.height);
                    break;
                }
            case CommandType::Draw:
                {
                    CHECK_VALID_GRAPHICS_PIPELINE(graphicsPipeline);
                    const DrawCommand& cmd = data.draw;
                    glDrawArrays(Convert<GLenum>(primitiveTopology), cmd.firstVertex, cmd.vertexCount);
                    break;
                }
            case CommandType::DrawIndexed:
                {
                    CHECK_VALID_GRAPHICS_PIPELINE(graphicsPipeline);
                    const DrawIndexedCommand& cmd = data.drawIndexed;
                    glDrawElements(Convert<GLenum>(primitiveTopology), cmd.indexCount, Convert<GLformat>(indexFormat).type, nullptr);
                    break;
                }
            case CommandType::DrawIndirect:
                {
                    CHECK_VALID_GRAPHICS_PIPELINE(graphicsPipeline);
                    const DrawIndirectCommand& cmd = data.drawIndirect;
                    const Buffer* buffer = cmd.buffer;
                    CHECK_VALID_INDIRECT_BUFFER(buffer);
                    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer->GetHandle());
                    glDrawArraysIndirect(Convert<GLenum>(primitiveTopology), reinterpret_cast<const void*>(cmd.offset));
                    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
                    break;
                }
            case CommandType::DrawIndexedIndirect:
                {
                    CHECK_VALID_GRAPHICS_PIPELINE(graphicsPipeline);
                    const DrawIndexedIndirectCommand& cmd = data.drawIndexedIndirect;
                    const Buffer* buffer = cmd.buffer;
                    CHECK_VALID_INDIRECT_BUFFER(buffer);
                    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer->GetHandle());
                    glDrawElementsIndirect(Convert<GLenum>(primitiveTopology), Convert<GLformat>(indexFormat).type, reinterpret_cast<const void*>(cmd.offset));
                    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
                    break;
                }
            case CommandType::Dispatch:
                {
                    CHECK_VALID_COMPUTE_PIPELINE(computePipeline);
                    const DispatchCommand& cmd = data.dispatch;
                    glDispatchCompute(cmd.groupCountX, cmd.groupCountY, cmd.groupCountZ);
                    break;
                }
            case CommandType::DispatchIndirect:
                {
                    CHECK_VALID_COMPUTE_PIPELINE(computePipeline);
                    const DispatchIndirectCommand& cmd = data.dispatchIndirect;
                    const Buffer* buffer = cmd.buffer;
                    CHECK_VALID_INDIRECT_BUFFER(buffer);
                    glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, buffer->GetHandle());
                    glDispatchComputeIndirect(static_cast<GLintptr>(cmd.offset));
                    glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);
                    break;
                }
            case CommandType::TextureBarrier:
                break;
            case CommandType::BufferBarrier:
                break;
            case CommandType::MemoryBarrier:
                break;
            case CommandType::BufferCopy:
                {
                    const BufferCopyCommand& cmd = data.bufferCopy;
                    glBindBuffer(GL_COPY_READ_BUFFER, cmd.srcBuffer->GetHandle());
                    glBindBuffer(GL_COPY_WRITE_BUFFER, cmd.dstBuffer->GetHandle());
                    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
                        static_cast<GLintptr>(cmd.srcOffset),
                        static_cast<GLintptr>(cmd.dstOffset),
                        static_cast<GLsizeiptr>(cmd.size));
                    glBindBuffer(GL_COPY_READ_BUFFER, 0);
                    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
                    break;
                }
            case CommandType::Blit:
                break;
            case CommandType::RenderPassBegin:
                {
                    const RenderPassBeginCommand& cmd = data.renderPassBegin;
                    const RenderPassBeginInfo* info = cmd.info;
                    BufferView colorAttachments(info->colorAttachments, info->colorAttachmentCount);

                    for (uint32_t i = 0; i < colorAttachments.Size(); i++)
                    {
                        const RenderPassAttachmentInfo& attachment = colorAttachments[i];
                        const Texture* texture = static_cast<const Texture*>(attachment.texture);

                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture->GetHandle(), 0);

                        if (attachment.resolveMode != ResolveMode::None && attachment.resolveTexture)
                        {
                            // Will be resolved at RenderPassEnd — store for later
                            // (you'd want to track these in local state)
                        }

                        switch (attachment.loadOp)
                        {
                        case LoadOperation::Load:
                            break;
                        case LoadOperation::Clear:
                            glClearBufferfv(GL_COLOR, i, reinterpret_cast<const GLfloat*>(&attachment.clearValue.color));
                            break;
                        case LoadOperation::DontCare:
                            break;
                        }
                    }

                    insideRenderPass = true;
                    break;
                }
            case CommandType::RenderPassEnd:
                {
                    insideRenderPass = false;
                }
            case CommandType::ExecuteCommandBuffers:
                {
                    const ExecuteCommandBuffersCommand& cmd = data.executeCommandBuffers;
                    BufferView commandBuffers(cmd.commandBuffers, cmd.commandBufferCount);
                    for (CommandBuffer* execCmdBuffer : commandBuffers)
                        Submit(execCmdBuffer, waitSemaphore, signalSemaphore, fence, waitStagesMask);
                    break;
                }
            }
        }

        // May assert BEFORE starting executing.
        NOVA_ASSERT(insideRenderPass, "A renderpass instance is begun and was never ended!");
    }

    void Queue::Submit(const Array<Nova::CommandBuffer*>& commandBuffers, const Array<Semaphore*>& waitSemaphores, const Array<Semaphore*>& signalSemaphores, Fence* fence, uint32_t waitStagesMask) const
    {

    }

    bool Queue::Present(const Swapchain& swapchain, const Nova::Semaphore* waitSemaphore, uint32_t imageIndex) const
    {
    }
}
