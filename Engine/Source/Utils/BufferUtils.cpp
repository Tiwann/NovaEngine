#include "BufferUtils.h"

#include "Rendering/Buffer.h"
#include "Rendering/Device.h"


namespace Nova
{
    Ref<Buffer> CreateVertexBuffer(Ref<Device>& device, const void* data, const size_t size)
    {
        BufferCreateInfo stagingBufferCreateInfo;
        stagingBufferCreateInfo.size = size;
        stagingBufferCreateInfo.usage = BufferUsage::StagingBuffer;

        Ref<Buffer> stagingBuffer = device->CreateBuffer(stagingBufferCreateInfo);
        stagingBuffer->CPUCopy(data, 0, size);

        BufferCreateInfo vertexBufferCreateInfo;
        vertexBufferCreateInfo.size = size;
        vertexBufferCreateInfo.usage = BufferUsage::VertexBuffer;

        Ref<Buffer> vertexBuffer = device->CreateBuffer(vertexBufferCreateInfo);
        stagingBuffer->GPUCopy(*vertexBuffer, 0, 0, size);
        stagingBuffer->Destroy();
        return vertexBuffer;
    }

    Ref<Buffer> CreateIndexBuffer(Ref<Device>& device, const void* data, const size_t size)
    {
        BufferCreateInfo stagingBufferCreateInfo;
        stagingBufferCreateInfo.device = device;
        stagingBufferCreateInfo.size = size;
        stagingBufferCreateInfo.usage = BufferUsage::StagingBuffer;

        Ref<Buffer> stagingBuffer = device->CreateBuffer(stagingBufferCreateInfo);
        stagingBuffer->CPUCopy(data, 0, size);

        BufferCreateInfo indexBufferCreateInfo;
        indexBufferCreateInfo.device = device;
        indexBufferCreateInfo.size = size;
        indexBufferCreateInfo.usage = BufferUsage::IndexBuffer;

        Ref<Buffer> indexBuffer = device->CreateBuffer(indexBufferCreateInfo);
        stagingBuffer->GPUCopy(*indexBuffer, 0, 0, size);
        stagingBuffer->Destroy();
        return indexBuffer;
    }
}
