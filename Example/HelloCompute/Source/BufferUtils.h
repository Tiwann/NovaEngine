#pragma once
#include "Rendering/Vulkan/Buffer.h"

namespace Nova
{
    Vulkan::Buffer CreateVertexBuffer(Rendering::Device& device, const void* data, size_t size);
    Vulkan::Buffer CreateIndexBuffer(Rendering::Device& device, const void* data, size_t size);
}
