#pragma once
#include "Rendering/CommandBuffer.h"
#include "Rendering/ShaderStage.h"

typedef struct VkCommandBuffer_T* VkCommandBuffer;

namespace Nova::Vulkan
{
    class Device;

    class CommandBuffer final : public Nova::CommandBuffer
    {
    public:
        CommandBuffer() = default;
        ~CommandBuffer() override = default;

        bool Allocate(const CommandBufferAllocateInfo& allocateInfo) override;
        void Free() override;
        void SetName(StringView name) override;
        bool Begin(const CommandBufferBeginInfo& beginInfo) override;
        void End() override;

        void ClearColor(const Color& color, uint32_t attachmentIndex) override;
        void ClearDepthStencil(float depth, uint32_t stencil, uint32_t attachmentIndex) override;
        void BindGraphicsPipeline(const Nova::GraphicsPipeline& pipeline) override;
        void BindComputePipeline(const Nova::ComputePipeline& pipeline) override;
        void BindVertexBuffer(const Nova::Buffer& vertexBuffer, size_t offset) override;
        void BindIndexBuffer(const Nova::Buffer& indexBuffer, size_t offset, Format indexFormat) override;
        void BindShaderBindingSet(const Nova::Shader& shader, const Nova::ShaderBindingSet& bindingSet) override;
        void BindMaterial(const Nova::Material& material) override;
        void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) override;
        void SetScissor(int32_t x, int32_t y, int32_t width, int32_t height) override;
        void Draw(size_t vertexCount, size_t instanceCount, size_t firstIndex, size_t firstInstance) override;
        void DrawIndexed(size_t count, size_t offset) override;
        void BeginRenderPass(const Nova::RenderPass& renderPass) override;
        void EndRenderPass() override;
        void PushConstants(const Nova::Shader& shader, const ShaderStageFlags stageFlags, const size_t offset, const size_t size, const void* values) override;
        void UpdateBuffer(const Nova::Buffer& buffer, size_t offset, size_t size, const void* data) override;

        void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;
        void DispatchIndirect(const Nova::Buffer& buffer, size_t offset) override;

        void BufferCopy(const Nova::Buffer& src, const Nova::Buffer& dest, size_t srcOffset, size_t destOffset, size_t size) override;
        void Blit(const Nova::Texture& src, const BlitRegion& srcRegion, const Nova::Texture& dest, const BlitRegion& destRegion, Filter filter) override;
        void Blit(const Nova::Texture& src, const Nova::Texture& dest, Filter filter) override;

        void ExecuteCommandBuffers(const Array<Nova::CommandBuffer*>& commandBuffers) override;


        VkCommandBuffer GetHandle() const;
        const VkCommandBuffer* GetHandlePtr() const;
    private:
        Device* m_Device = nullptr;
        VkCommandBuffer m_Handle = nullptr;
    };
}
