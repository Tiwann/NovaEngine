#pragma once
#include "Rendering/Vulkan/Buffer.h"
#include "Runtime/Ref.h"

namespace Nova
{
    Vulkan::Buffer CreateVertexBuffer(const Ref<Rendering::Device>& device, const void* data, size_t size);
    Vulkan::Buffer CreateIndexBuffer(const Ref<Rendering::Device>& device, const void* data, size_t size);
}
