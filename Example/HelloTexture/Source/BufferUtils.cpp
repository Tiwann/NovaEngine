#include "BufferUtils.h"

#include <algorithm>

namespace Nova
{
    Vulkan::Buffer CreateVertexBuffer(Rendering::Device& device, const void* data, size_t size)
    {
        Rendering::BufferCreateInfo stagingBufferCreateInfo;
        stagingBufferCreateInfo.device = &device;
        stagingBufferCreateInfo.size = size;
        stagingBufferCreateInfo.usage = Rendering::BufferUsage::StagingBuffer;

        Vulkan::Buffer stagingBuffer;
        stagingBuffer.Initialize(stagingBufferCreateInfo);
        stagingBuffer.CopyData(data, 0, size);

        Rendering::BufferCreateInfo vertexBufferCreateInfo;
        vertexBufferCreateInfo.device = &device;
        vertexBufferCreateInfo.size = size;
        vertexBufferCreateInfo.usage = Rendering::BufferUsage::VertexBuffer;

        Vulkan::Buffer vertexBuffer;
        vertexBuffer.Initialize(vertexBufferCreateInfo);
        stagingBuffer.CopyTo(vertexBuffer, 0, 0, size);
        stagingBuffer.Destroy();
        return std::move(vertexBuffer);
    }

    Vulkan::Buffer CreateIndexBuffer(Rendering::Device& device, const void* data, size_t size)
    {
        Rendering::BufferCreateInfo stagingBufferCreateInfo;
        stagingBufferCreateInfo.device = &device;
        stagingBufferCreateInfo.size = size;
        stagingBufferCreateInfo.usage = Rendering::BufferUsage::StagingBuffer;

        Vulkan::Buffer stagingBuffer;
        stagingBuffer.Initialize(stagingBufferCreateInfo);
        stagingBuffer.CopyData(data, 0, size);

        Rendering::BufferCreateInfo indexBufferCreateInfo;
        indexBufferCreateInfo.device = &device;
        indexBufferCreateInfo.size = size;
        indexBufferCreateInfo.usage = Rendering::BufferUsage::IndexBuffer;

        Vulkan::Buffer indexBuffer;
        indexBuffer.Initialize(indexBufferCreateInfo);
        stagingBuffer.CopyTo(indexBuffer, 0, 0, size);
        stagingBuffer.Destroy();
        return std::move(indexBuffer);
    }
}
