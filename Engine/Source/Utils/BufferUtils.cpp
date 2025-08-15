#include "BufferUtils.h"

#include "Rendering/Buffer.h"
#include "Rendering/Device.h"


namespace Nova
{
    Ref<Rendering::Buffer> CreateVertexBuffer(const Ref<Rendering::Device>& device, const void* data, const size_t size)
    {
        Rendering::BufferCreateInfo stagingBufferCreateInfo;
        stagingBufferCreateInfo.device = device;
        stagingBufferCreateInfo.size = size;
        stagingBufferCreateInfo.usage = Rendering::BufferUsage::StagingBuffer;

        Ref<Rendering::Buffer> stagingBuffer = device->CreateBuffer(stagingBufferCreateInfo);
        stagingBuffer->CPUCopy(data, 0, size);

        Rendering::BufferCreateInfo vertexBufferCreateInfo;
        vertexBufferCreateInfo.device = device;
        vertexBufferCreateInfo.size = size;
        vertexBufferCreateInfo.usage = Rendering::BufferUsage::VertexBuffer;

        Ref<Rendering::Buffer> vertexBuffer = device->CreateBuffer(vertexBufferCreateInfo);
        stagingBuffer->GPUCopy(*vertexBuffer, 0, 0, size);
        stagingBuffer->Destroy();
        return vertexBuffer;
    }

    Ref<Rendering::Buffer> CreateIndexBuffer(const Ref<Rendering::Device>& device, const void* data, const size_t size)
    {
        Rendering::BufferCreateInfo stagingBufferCreateInfo;
        stagingBufferCreateInfo.device = device;
        stagingBufferCreateInfo.size = size;
        stagingBufferCreateInfo.usage = Rendering::BufferUsage::StagingBuffer;

        Ref<Rendering::Buffer> stagingBuffer = device->CreateBuffer(stagingBufferCreateInfo);
        stagingBuffer->CPUCopy(data, 0, size);

        Rendering::BufferCreateInfo indexBufferCreateInfo;
        indexBufferCreateInfo.device = device;
        indexBufferCreateInfo.size = size;
        indexBufferCreateInfo.usage = Rendering::BufferUsage::IndexBuffer;

        Ref<Rendering::Buffer> indexBuffer = device->CreateBuffer(indexBufferCreateInfo);
        stagingBuffer->GPUCopy(*indexBuffer, 0, 0, size);
        stagingBuffer->Destroy();
        return indexBuffer;
    }
}
