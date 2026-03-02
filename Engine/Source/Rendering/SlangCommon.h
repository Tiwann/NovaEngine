#pragma once
#include "ShaderCompileTarget.h"
#include "ShaderStage.h"
#include "BindingType.h"
#include "RenderDeviceType.h"
#include "Containers/StringView.h"
#include <slang/slang.h>
#include <slang/slang-com-ptr.h>


namespace Nova
{
    SlangCompileTarget GetCompileTarget(ShaderCompileTarget target, const RenderDeviceType deviceType);
    ShaderStageFlagBits GetStage(SlangStage stage);
    BindingType GetBindingType(slang::BindingType bindingType);
    StringView GetErrorString(const Slang::ComPtr<slang::IBlob>& blob);;
}
