#pragma once
#include "Rendering/CommandBuffer.h"

typedef struct VkCommandBuffer_T* VkCommandBuffer;

namespace Nova::Vulkan
{
    class CommandPool;
    class Device;

    class CommandBuffer : public Rendering::CommandBuffer
    {
    public:
        bool Allocate(const Rendering::CommandBufferAllocateInfo& allocateInfo) override;
        void Free() override;
        bool Begin(const Rendering::CommandBufferBeginInfo& beginInfo) override;
        void End() override;

        void ClearColor(const Color& color) override;
        void ClearDepth(float depth, uint32_t stencil) override;
        void BindGraphicsPipeline(const Rendering::GraphicsPipeline& pipeline) override;
        void BindVertexBuffer(const Rendering::Buffer& vertexBuffer, size_t offset) override;
        void BindIndexBuffer(const Rendering::Buffer& indexBuffer, size_t offset, Format indexFormat) override;
        void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) override;
        void SetScissor(int32_t x, int32_t y, int32_t width, int32_t height) override;
        void DrawIndexed(size_t count, size_t offset) override;

        VkCommandBuffer GetHandle() const;
        const VkCommandBuffer* GetHandlePtr() const;

    private:
        Device* m_Device = nullptr;
        VkCommandBuffer m_Handle = nullptr;
    };
}
