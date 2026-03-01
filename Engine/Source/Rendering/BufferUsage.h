#pragma once

namespace Nova
{
    enum class BufferUsage
    {
        None,
        VertexBuffer,
        IndexBuffer,
        UniformBuffer,
        StorageBuffer,
        StagingBuffer,
        IndirectBuffer
    };
}
