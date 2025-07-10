#pragma once
#include "Runtime/Types.h"

typedef enum VkDescriptorType : Nova::i32 VkDescriptorType;
typedef Nova::u32 VkFlags;
typedef VkFlags VkShaderStageFlags;
typedef enum SlangStage : Nova::u32 SlangStage;

namespace slang { enum class BindingType : Nova::u32; }

namespace Nova
{
    static VkDescriptorType ToVulkanDescriptorType(slang::BindingType BindingType);
    static VkShaderStageFlags ToVulkanShaderStage(SlangStage Stage);
    static enum class DescriptorType ToNovaDescriptorType(slang::BindingType BindingType);
}
