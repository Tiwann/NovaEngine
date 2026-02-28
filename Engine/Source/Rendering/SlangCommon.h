#pragma once
#include "ShaderTarget.h"
#include "ShaderStage.h"
#include "BindingType.h"
#include "Containers/StringView.h"
#include <slang/slang.h>
#include <slang/slang-com-ptr.h>

namespace Nova
{
    SlangCompileTarget GetCompileTarget(ShaderTarget target);
    ShaderStageFlagBits GetStage(SlangStage stage);
    BindingType GetBindingType(slang::BindingType bindingType);
    StringView GetErrorString(const Slang::ComPtr<slang::IBlob>& blob);;
}