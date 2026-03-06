#pragma once
#include "Containers/Array.h"
#include "Runtime/Ref.h"

namespace Nova
{
    class RenderDevice;
    class Buffer;

    Ref<Buffer> CreateVertexBuffer(Ref<RenderDevice>& device, const void* data, size_t size);
    Ref<Buffer> CreateVertexBuffer(Ref<RenderDevice>& device, const Array<uint8_t>& data);
    Ref<Buffer> CreateIndexBuffer(Ref<RenderDevice>& device, const void* data, size_t size);
    Ref<Buffer> CreateIndexBuffer(Ref<RenderDevice>& device, const Array<uint8_t>& data);
}
