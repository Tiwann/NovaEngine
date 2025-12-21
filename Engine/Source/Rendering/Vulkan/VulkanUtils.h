#pragma once
#include "Rendering/ResourceBarrier.h"
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>


namespace Nova
{
    struct TextureBarrier;
    struct BufferBarrier;
}

namespace Nova::Vulkan
{
    VkAccessFlags2 GetSourceAccessFlags(VkImageLayout layout);
    VkAccessFlags2 GetDestAccessFlags(VkImageLayout layout);
    VkPipelineStageFlags GetPipelineStageFlags(AccessFlags accessFlags);
    VkImageMemoryBarrier MakeTextureBarrier(const Nova::TextureBarrier& barrier);
    VkBufferMemoryBarrier MakeBufferBarrier(const Nova::BufferBarrier& barrier);
}
