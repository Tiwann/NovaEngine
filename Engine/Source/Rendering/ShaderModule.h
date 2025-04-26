#pragma once
#include "ShaderStage.h"
#include <type_traits>


struct ISlangBlob;

namespace slang
{
    struct IEntryPoint;
    typedef ISlangBlob IBlob;
}

typedef struct VkShaderModule_T *VkShaderModule;



namespace Nova
{
    template<typename HandleType> requires std::is_convertible_v<HandleType, void*>
    struct ShaderModule
    {
        ShaderStage Stage = ShaderStage::None;
        slang::IEntryPoint* EntryPoint = nullptr;
        slang::IBlob* CompiledCode = nullptr;
        HandleType Handle = nullptr;
    };

    using VulkanShaderModule = ShaderModule<VkShaderModule>;
    using D3D12ShaderModule = ShaderModule<void*>;
}