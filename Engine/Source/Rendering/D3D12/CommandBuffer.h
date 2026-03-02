#pragma once
#include "Rendering/CommandBuffer.h"

struct ID3D12GraphicsCommandList10;

namespace Nova::D3D12
{
    class RenderDevice;

    class CommandBuffer final : public Nova::CommandBuffer
    {
    public:
        bool Allocate(const CommandBufferAllocateInfo& allocateInfo) override;
        void Free() override;
        void SetName(StringView name) override;
        bool Begin(const CommandBufferBeginInfo& beginInfo) override;
        void End() override;
        void ClearColor(const Color& color, uint32_t attachmentIndex) override;
        void ClearDepthStencil(float depth, uint32_t stencil) override;
        void BindGraphicsPipeline(const Nova::GraphicsPipeline& pipeline) override;
        void BindComputePipeline(const Nova::ComputePipeline& pipeline) override;
        void BindVertexBuffer(const Nova::Buffer& vertexBuffer, size_t offset) override;
        void BindIndexBuffer(const Nova::Buffer& indexBuffer, size_t offset, Format indexFormat) override;
        void BindShaderBindingSet(const Nova::Shader& shader, const Nova::ShaderBindingSet& bindingSet) override;
        void BindMaterial(const Nova::Material& material) override;
        void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) override;
        void SetScissor(int32_t x, int32_t y, int32_t width, int32_t height) override;
        void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
        void DrawIndexed(uint32_t indexCount, uint32_t instanceCount) override;
        void DrawIndirect(const Nova::Buffer& buffer, size_t offset, uint32_t drawCount) override;
        void BeginRenderPass(const RenderPassBeginInfo& beginInfo) override;
        void EndRenderPass() override;
        void PushConstants(const Nova::Shader& shader, ShaderStageFlags stageFlags, size_t offset, size_t size,const void* values) override;
        void UpdateBuffer(const Nova::Buffer& buffer, size_t offset, size_t size, const void* data) override;
        void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;
        void DispatchIndirect(const Nova::Buffer& buffer, size_t offset) override;
        void BufferCopy(const Nova::Buffer& src, const Nova::Buffer& dest, size_t srcOffset, size_t destOffset,size_t size) override;
        void Blit(const Nova::Texture& src, const Nova::BlitRegion& srcRegion, const Nova::Texture& dest, const Nova::BlitRegion& destRegion, Filter filter) override;
        void Blit(const Nova::Texture& src, const Nova::Texture& dest, Filter filter) override;
        void ExecuteCommandBuffers(const Array<Nova::CommandBuffer*>& commandBuffers) override;
        void TextureBarrier(const Nova::TextureBarrier& barrier) override;
        void BufferBarrier(const Nova::BufferBarrier& barrier) override;
        void MemoryBarrier(const Nova::MemoryBarrier& barrier) override;

        ID3D12GraphicsCommandList10* GetHandle() { return m_Handle; }
        const ID3D12GraphicsCommandList10* GetHandle() const { return m_Handle; }

    private:
        RenderDevice* m_Device = nullptr;
        ID3D12GraphicsCommandList10* m_Handle = nullptr;
    };
}
