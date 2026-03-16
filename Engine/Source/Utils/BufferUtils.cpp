#include "BufferUtils.h"
#include "Rendering/Buffer.h"
#include "Rendering/CommandBuffer.h"
#include "Rendering/Fence.h"
#include "Rendering/Queue.h"
#include "Rendering/RenderDevice.h"
#include "Runtime/Common.h"

namespace Nova::BufferUtils
{
    Ref<Buffer> CreateStagingBuffer(Ref<RenderDevice>& device, const void* data, size_t size)
    {
        BufferCreateInfo bufferCreateInfo;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = BufferUsage::StagingBuffer;
        Ref<Buffer> stagingBuffer = device->CreateBuffer(bufferCreateInfo);
        if (!stagingBuffer) return nullptr;

        void* mappedData = stagingBuffer->Map();
        Memory::Memcpy(mappedData, data, size);
        stagingBuffer->Unmap(mappedData);
        return stagingBuffer;
    }

    Ref<Buffer> CreateVertexBuffer(Ref<RenderDevice>& device, const void* data, const size_t size)
    {
        Ref<Buffer> stagingBuffer = CreateStagingBuffer(device, data, size);
        if (!stagingBuffer) return nullptr;
        NOVA_DEFER(stagingBuffer, &Buffer::Destroy);

        BufferCreateInfo bufferCreateInfo;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = BufferUsage::VertexBuffer;
        Ref<Buffer> vertexBuffer = device->CreateBuffer(bufferCreateInfo);
        if (!vertexBuffer) return nullptr;

        Ref<CommandBuffer> cmdBuffer = device->CreateTransferCommandBuffer();
        if (!cmdBuffer)
        {
            vertexBuffer->Destroy();
            return nullptr;
        }
        NOVA_DEFER(cmdBuffer, &CommandBuffer::Free);

        cmdBuffer->Begin({ CommandBufferUsageFlagBits::OneTimeSubmit });
        cmdBuffer->BufferCopy(*stagingBuffer, *vertexBuffer, 0, 0, size);
        cmdBuffer->End();

        Ref<Fence> fence = device->CreateFence();
        const Queue* transferQueue = device->GetTransferQueue();
        transferQueue->Submit(cmdBuffer, nullptr, nullptr, fence);
        fence->Wait(FENCE_WAIT_INFINITE);

        return vertexBuffer;
    }

    Ref<Buffer> CreateIndexBuffer(Ref<RenderDevice>& device, const void* data, const size_t size)
    {
        Ref<Buffer> stagingBuffer = CreateStagingBuffer(device, data, size);
        if (!stagingBuffer) return nullptr;
        NOVA_DEFER(stagingBuffer, &Buffer::Destroy);

        BufferCreateInfo indexBufferCreateInfo;
        indexBufferCreateInfo.device = device;
        indexBufferCreateInfo.size = size;
        indexBufferCreateInfo.usage = BufferUsage::IndexBuffer;
        Ref<Buffer> indexBuffer = device->CreateBuffer(indexBufferCreateInfo);
        if (!indexBuffer) return nullptr;

        Ref<CommandBuffer> cmdBuffer = device->CreateTransferCommandBuffer();
        if (!cmdBuffer)
        {
            indexBuffer->Destroy();
            return nullptr;
        }
        NOVA_DEFER(cmdBuffer, &CommandBuffer::Free);

        cmdBuffer->Begin({ CommandBufferUsageFlagBits::OneTimeSubmit });
        cmdBuffer->BufferCopy(*stagingBuffer, *indexBuffer, 0, 0, size);
        cmdBuffer->End();

        Ref<Fence> fence = device->CreateFence();
        const Queue* transferQueue = device->GetTransferQueue();
        transferQueue->Submit(cmdBuffer, nullptr, nullptr, fence);
        fence->Wait(FENCE_WAIT_INFINITE);
        return indexBuffer;
    }
}
