#pragma once
#include "Runtime/Ref.h"

namespace Nova
{
    class Buffer;
    class RenderDevice;

    namespace BufferUtils
    {
        Ref<Buffer> CreateStagingBuffer(Ref<RenderDevice>& device, const void* data, size_t size);
        Ref<Buffer> CreateVertexBuffer(Ref<RenderDevice>& device, const void* data, size_t size);
        Ref<Buffer> CreateIndexBuffer(Ref<RenderDevice>& device, const void* data, size_t size);

        template<typename T, size_t N>
        Ref<Buffer> CreateVertexBuffer(Ref<RenderDevice>& device, const T(&data)[N])
        {
            return CreateVertexBuffer(device, data, sizeof(data[0]) * N);
        }

        template<typename T, size_t N>
        Ref<Buffer> CreateIndexBuffer(Ref<RenderDevice>& device, const T(&data)[N])
        {
            return CreateIndexBuffer(device, data, sizeof(data[0]) * N);
        }
    }
}