#pragma once

namespace Nova
{
    enum class BlendFactor
    {
        Zero,
        One,
        SourceColor,
        OneMinusSourceColor,
        DestColor,
        OneMinusDestColor,
        SourceAlpha,
        OneMinusSourceAlpha,
        DestAlpha,
        OneMinusDestAlpha,
        ConstantColor,
        OnMinusConstantColor,
        ConstantAlpha,
        OneMinusConstantAlpha,
        SourceAlphaSaturated,
        Source1Color,
        OneMinusSource1Color,
        Source1Alpha,
        OneMinusSource1Alpha,
    };
}