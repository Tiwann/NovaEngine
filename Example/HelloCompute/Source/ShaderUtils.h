#pragma once
#include "Containers/Array.h"
#include "Containers/StringView.h"

namespace Nova
{
    void CompileShaderToSpirV(StringView filepath, Array<uint32_t>& vertSpirv, Array<uint32_t>& fragSpirv);
}
