#pragma once
#include "Containers/StringView.h"
#include "ShaderStage.h"

namespace Nova::Rendering
{
    struct ShaderEntryPoint
    {
        StringView name;
        ShaderStageFlagBits stage;
    };
}
