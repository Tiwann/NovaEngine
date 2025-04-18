#pragma once
#include "Containers/Array.h"
#include <vulkan/vulkan.h>

namespace Nova
{
    struct DescriptorSetInfo
    {
        Array<VkDescriptorSetLayoutBinding> Bindings;
        VkDescriptorSetLayout DescriptorSetLayout = nullptr;
    };
}