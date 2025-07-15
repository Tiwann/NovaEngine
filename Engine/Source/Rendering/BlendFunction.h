#pragma once
#include "BlendFactor.h"
#include "BlendOperation.h"

namespace Nova
{
    struct BlendFunction
    {
        BlendFactor ColorSource;
        BlendFactor ColorDest;
        BlendOperation ColorOperation;
        BlendFactor AlphaSource;
        BlendFactor AlphaDest;
        BlendOperation AlphaOperation;
    };

    constexpr BlendFunction AlphaBlend = { BlendFactor::SourceAlpha,
            BlendFactor::OneMinusSourceAlpha,
            BlendOperation::Add,
            BlendFactor::One,
            BlendFactor::Zero,
            BlendOperation::Add };


    /*constexpr BlendFunction AdditiveBlend = { BlendFactor::SourceAlpha,
            BlendFactor::OneMinusSourceAlpha,
            BlendOperation::Add,
            BlendFactor::One,
            BlendFactor::Zero,
            BlendOperation::Add };*/

    inline BlendFunction MakeBlendFunction(BlendFactor Source, BlendFactor Dest, BlendOperation Operation)
    {
        return { Source, Dest, Operation, Source, Dest, Operation };
    }
}
