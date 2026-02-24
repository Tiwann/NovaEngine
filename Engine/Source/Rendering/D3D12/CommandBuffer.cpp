#include "CommandBuffer.h"
#include "Device.h"
#include "CommandPool.h"
#include "Buffer.h"
#include "Containers/StringConversion.h"
#include "Rendering/ResourceBarrier.h"

#pragma push_macro("MemoryBarrier")
#undef MemoryBarrier
#include <directx/d3dx12.h>
#pragma pop_macro("MemoryBarrier")

namespace Nova::D3D12
{
    bool CommandBuffer::Allocate(const CommandBufferAllocateInfo& allocateInfo)
    {
        if (!allocateInfo.device) return false;
        if (!allocateInfo.commandPool) return false;

        Device* device = static_cast<Device*>(allocateInfo.device);
        CommandPool* commandPool = static_cast<CommandPool*>(allocateInfo.commandPool);
        ID3D12Device13* deviceHandle = device->GetHandle();
        ID3D12CommandAllocator* commandAllocator = commandPool->GetHandle();

        ID3D12CommandList* commandBuffer = nullptr;
        if (DX_FAILED(deviceHandle->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandBuffer))))
            return false;

        if (m_Handle) m_Handle->Release();
        if (DX_FAILED(commandBuffer->QueryInterface(IID_PPV_ARGS(&m_Handle))))
            return false;

        m_Handle->Close();

        m_Device = device;
        return true;
    }

    void CommandBuffer::Free()
    {
        m_Handle->Release();
    }

    void CommandBuffer::SetName(StringView name)
    {
        m_Handle->SetName(*StringConvertToWide((name)));
    }

    bool CommandBuffer::Begin(const CommandBufferBeginInfo& beginInfo)
    {
        CommandPool* cmdPool = static_cast<CommandPool*>(m_CommandPool);
        if (DX_FAILED(m_Handle->Reset(cmdPool->GetHandle(), nullptr)))
            return false;
        return true;
    }

    void CommandBuffer::End()
    {
        m_Handle->Close();
    }

    void CommandBuffer::ClearColor(const Color& color, uint32_t attachmentIndex)
    {
        // Need renderpass
        //m_Handle->ClearRenderTargetView()
    }

    void CommandBuffer::ClearDepthStencil(float depth, uint32_t stencil, uint32_t attachmentIndex)
    {
    }

    void CommandBuffer::BindGraphicsPipeline(const GraphicsPipeline& pipeline)
    {

    }

    void CommandBuffer::BindComputePipeline(const ComputePipeline& pipeline)
    {
    }

    void CommandBuffer::BindVertexBuffer(const Nova::Buffer& vertexBuffer, size_t offset)
    {
    }

    void CommandBuffer::BindIndexBuffer(const Nova::Buffer& indexBuffer, size_t offset, Format indexFormat)
    {
    }

    void CommandBuffer::BindShaderBindingSet(const Shader& shader, const ShaderBindingSet& bindingSet)
    {
    }

    void CommandBuffer::BindMaterial(const Material& material)
    {
    }

    void CommandBuffer::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
    {
        D3D12_VIEWPORT viewport { x, y, width, height, minDepth, maxDepth };
        m_Handle->RSSetViewports(1, &viewport);
    }

    void CommandBuffer::SetScissor(int32_t x, int32_t y, int32_t width, int32_t height)
    {
        D3D12_RECT scissorRect { x, y, width, height };
        m_Handle->RSSetScissorRects(1, &scissorRect);
    }

    void CommandBuffer::Draw(size_t vertexCount, size_t instanceCount, size_t firstIndex, size_t firstInstance)
    {
        m_Handle->IASetIndexBuffer(nullptr);
        m_Handle->DrawInstanced(vertexCount, instanceCount, firstIndex, firstInstance);
    }

    void CommandBuffer::DrawIndexed(size_t count, size_t offset)
    {
        m_Handle->DrawIndexedInstanced(count, 1, 0, offset, 0);
    }

    void CommandBuffer::BeginRenderPass(const RenderPass& renderPass)
    {
    }

    void CommandBuffer::EndRenderPass()
    {
    }

    void CommandBuffer::PushConstants(const Shader& shader, ShaderStageFlags stageFlags, size_t offset, size_t size,const void* values)
    {
    }

    void CommandBuffer::UpdateBuffer(const Nova::Buffer& buffer, size_t offset, size_t size, const void* data)
    {

    }

    void CommandBuffer::Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
    {
        m_Handle->Dispatch(groupCountX, groupCountY, groupCountZ);
    }

    void CommandBuffer::DispatchIndirect(const Nova::Buffer& buffer, size_t offset)
    {

    }

    void CommandBuffer::BufferCopy(const Nova::Buffer& src, const Nova::Buffer& dest, size_t srcOffset, size_t destOffset, size_t size)
    {
    }

    void CommandBuffer::Blit(const Texture& src, const BlitRegion& srcRegion, const Texture& dest, const BlitRegion& destRegion, Filter filter)
    {
    }

    void CommandBuffer::Blit(const Texture& src, const Texture& dest, Filter filter)
    {
    }

    void CommandBuffer::ExecuteCommandBuffers(const Array<Nova::CommandBuffer*>& commandBuffers)
    {

    }

    void CommandBuffer::TextureBarrier(const Nova::TextureBarrier& barrier)
    {
    }

    void CommandBuffer::BufferBarrier(const Nova::BufferBarrier& barrier)
    {
    }

    void CommandBuffer::MemoryBarrier(const Nova::MemoryBarrier& barrier)
    {
    }
}
