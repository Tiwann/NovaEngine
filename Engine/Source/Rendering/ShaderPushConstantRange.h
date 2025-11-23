#pragma once
#include "ShaderStage.h"
#include <cstddef>


namespace Nova
{
    struct ShaderPushConstantRange
    {
        size_t offset = 0;
        size_t size = 0;
        ShaderStageFlags stageFlags = ShaderStageFlagBits::None;
    };
}
