#pragma once
#include "Runtime/Ref.h"

namespace Nova
{
    class RenderDevice;
    class Buffer;

    Ref<Buffer> CreateVertexBuffer(Ref<RenderDevice>& device, const void* data, size_t size);
    Ref<Buffer> CreateIndexBuffer(Ref<RenderDevice>& device, const void* data, size_t size);
}
