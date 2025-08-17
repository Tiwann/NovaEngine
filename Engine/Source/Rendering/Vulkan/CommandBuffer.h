#pragma once
#include "Rendering/CommandBuffer.h"
#include "Rendering/ShaderStage.h"

typedef struct VkCommandBuffer_T* VkCommandBuffer;

namespace Nova::Vulkan
{
    class CommandPool;
    class Device;

    class CommandBuffer : public Rendering::CommandBuffer
    {
    public:
        CommandBuffer() = default;
        ~CommandBuffer() override = default;

        bool Allocate(const Rendering::CommandBufferAllocateInfo& allocateInfo) override;
        void Free() override;
        void SetName(StringView name) override;
        bool Begin(const Rendering::CommandBufferBeginInfo& beginInfo) override;
        void End() override;

        void ClearColor(const Color& color, uint32_t attachmentIndex) override;
        void ClearDepth(float depth, uint32_t stencil) override;
        void BindGraphicsPipeline(const Rendering::GraphicsPipeline& pipeline) override;
        void BindComputePipeline(const Rendering::ComputePipeline& pipeline) override;
        void BindVertexBuffer(const Rendering::Buffer& vertexBuffer, size_t offset) override;
        void BindIndexBuffer(const Rendering::Buffer& indexBuffer, size_t offset, Format indexFormat) override;
        void BindShaderBindingSet(const Rendering::Shader& shader, const Rendering::ShaderBindingSet& bindingSet) override;
        void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) override;
        void SetScissor(int32_t x, int32_t y, int32_t width, int32_t height) override;
        void DrawIndexed(size_t count, size_t offset) override;
        void BeginRenderPass(const Rendering::RenderPass& renderPass) override;
        void EndRenderPass() override;
        void PushConstants(const Ref<Rendering::Shader>& shader, const ShaderStageFlags stageFlags, const size_t offset, const size_t size, const void* values) override;
        void UpdateBuffer(const Rendering::Buffer& buffer, size_t offset, size_t size, const void* data) override;

        void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;
        void DispatchIndirect(const Rendering::Buffer& buffer, size_t offset) override;

        void BufferCopy(const Rendering::Buffer& src, const Rendering::Buffer& dest, size_t srcOffset, size_t destOffset, size_t size) override;
        void Blit(const Rendering::Texture& src, const Rendering::BlitRegion& srcRegion, const Rendering::Texture& dest, const Rendering::BlitRegion& destRegion, Filter filter) override;
        void Blit(const Rendering::Texture& src, const Rendering::Texture& dest, Filter filter) override;

        void ExecuteCommandBuffers(const Array<Rendering::CommandBuffer*>& commandBuffers) override;


        VkCommandBuffer GetHandle() const;
        const VkCommandBuffer* GetHandlePtr() const;
    private:
        Device* m_Device = nullptr;
        VkCommandBuffer m_Handle = nullptr;
    };
}
