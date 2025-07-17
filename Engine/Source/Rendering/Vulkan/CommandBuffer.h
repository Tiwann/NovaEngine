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
        void ClearDepth(float depth, uint8_t stencil) override;
        void BindVertexBuffer(const Rendering::Buffer& vertexBuffer, size_t offset) override;
        void BindIndexBuffer(const Rendering::Buffer& indexBuffer, size_t offset, Format indexFormat) override;

        VkCommandBuffer GetHandle() const;
        const VkCommandBuffer* GetHandlePtr() const;

    private:
        Device* m_Device = nullptr;
        VkCommandBuffer m_Handle = nullptr;
    };
}
