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
    VkPipelineStageFlags GetSourcePipelineStageFlags(ResourceAccessFlags accessFlags);
    VkPipelineStageFlags GetDestPipelineStageFlags(ResourceAccessFlags accessFlags);
    VkImageMemoryBarrier MakeTextureBarrier(const TextureBarrier& barrier);
    VkBufferMemoryBarrier MakeBufferBarrier(const BufferBarrier& barrier);
}
