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

        constexpr static BlendFunction AlphaBlend()
        {
            static BlendFunction alphaBlend = {
                BlendFactor::SourceAlpha,
                BlendFactor::OneMinusSourceAlpha,
                BlendOperation::Add,
                BlendFactor::One,
                BlendFactor::Zero,
                BlendOperation::Add
            };
            return alphaBlend;
        }

        constexpr static BlendFunction AdditiveBlend()
        {
            static BlendFunction additiveBlend = {
                BlendFactor::SourceAlpha,
                BlendFactor::OneMinusSourceAlpha,
                BlendOperation::Add,
                BlendFactor::One,
                BlendFactor::Zero,
                BlendOperation::Add
            };
            return additiveBlend;
        }
    };

    constexpr BlendFunction MakeBlendFunction(BlendFactor Source, BlendFactor Dest, BlendOperation Operation)
    {
        return { Source, Dest, Operation, Source, Dest, Operation };
    }
}
