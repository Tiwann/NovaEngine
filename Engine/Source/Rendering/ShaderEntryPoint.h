#pragma once
#include "Containers/StringView.h"
#include "ShaderStage.h"

namespace Nova
{
    struct ShaderEntryPoint
    {
        StringView name;
        ShaderStageFlagBits stage;

        static constexpr const ShaderEntryPoint& DefaultVertex()
        {
            static const ShaderEntryPoint entryPoint { "vert", ShaderStageFlagBits::Vertex };
            return entryPoint;
        }

        static constexpr const ShaderEntryPoint& DefaultFragment()
        {
            static const ShaderEntryPoint entryPoint { "frag", ShaderStageFlagBits::Fragment };
            return entryPoint;
        }

        static constexpr const ShaderEntryPoint& DefaultCompute()
        {
            static const ShaderEntryPoint entryPoint { "compute", ShaderStageFlagBits::Compute };
            return entryPoint;
        }

        static constexpr const ShaderEntryPoint* Default()
        {
            static const ShaderEntryPoint entryPoints[]
            {
                DefaultVertex(),
                DefaultFragment(),
            };
            return entryPoints;
        }
    };
}
