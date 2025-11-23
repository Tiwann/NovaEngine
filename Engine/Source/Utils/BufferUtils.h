#pragma once
#include "Runtime/Ref.h"

namespace Nova
{
    class Device;
    class Buffer;

    Ref<Buffer> CreateVertexBuffer(Ref<Device>& device, const void* data, size_t size);
    Ref<Buffer> CreateIndexBuffer(Ref<Device>& device, const void* data, size_t size);
}
