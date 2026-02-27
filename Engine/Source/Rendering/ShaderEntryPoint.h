#pragma once
#include "Containers/StringView.h"
#include "ShaderStage.h"

namespace Nova
{
    struct ShaderEntryPoint
    {
        StringView name;
        ShaderStageFlagBits stage;

        static const ShaderEntryPoint& DefaultVertex()
        {
            static ShaderEntryPoint entryPoint { "vert", ShaderStageFlagBits::Vertex };
            return entryPoint;
        }

        static const ShaderEntryPoint& DefaultFragment()
        {
            static ShaderEntryPoint entryPoint { "frag", ShaderStageFlagBits::Fragment };
            return entryPoint;
        }

        static const ShaderEntryPoint& DefaultCompute()
        {
            static ShaderEntryPoint entryPoint { "compute", ShaderStageFlagBits::Compute };
            return entryPoint;
        }
    };
}
