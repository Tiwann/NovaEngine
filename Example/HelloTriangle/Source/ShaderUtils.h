#pragma once
#include "Containers/Array.h"
#include "Containers/StringView.h"

namespace Nova
{
    void CompileShaderToSpirv(StringView filepath, StringView moduleName, Array<uint32_t>& vertSpirv, Array<uint32_t>& fragSpirv);
    void CompileShaderToSpirv(StringView filepath, StringView moduleName, Array<uint32_t>& computeSpirv);
}
