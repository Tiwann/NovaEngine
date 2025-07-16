#pragma once
#include "BlendFactor.h"
#include "BlendOperation.h"

namespace Nova
{
    struct BlendFunction
    {
        BlendFactor colorSource;
        BlendFactor colorDest;
        BlendOperation colorOp;
        BlendFactor alphaSource;
        BlendFactor alphaDest;
        BlendOperation alphaOp;
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

    constexpr BlendFunction MakeBlendFunction(BlendFactor Source, BlendFactor Dest, BlendOperation Operation)
    {
        return { Source, Dest, Operation, Source, Dest, Operation };
    }
}
