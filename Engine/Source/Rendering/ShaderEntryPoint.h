#pragma once
#include "Containers/StringView.h"
#include "ShaderStage.h"

namespace Nova
{
    struct ShaderEntryPoint
    {
        StringView name;
        ShaderStageFlagBits stage;
    };
}
