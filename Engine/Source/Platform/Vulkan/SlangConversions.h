#pragma once
#include <vulkan/vulkan.h>
#include <slang/slang.h>

namespace Nova
{
    static VkDescriptorType ConvertSlangDescriptorType(slang::BindingType BindingType);
    static VkShaderStageFlags ConvertSlangShaderStage(SlangStage Stage);
}
