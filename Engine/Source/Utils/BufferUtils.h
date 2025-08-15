#pragma once
#include "Runtime/Ref.h"

namespace Nova
{
    namespace Rendering
    {
        class Device;
        class Buffer;
    }

    Ref<Rendering::Buffer> CreateVertexBuffer(const Ref<Rendering::Device>& device, const void* data, size_t size);
    Ref<Rendering::Buffer> CreateIndexBuffer(const Ref<Rendering::Device>& device, const void* data, size_t size);
}
