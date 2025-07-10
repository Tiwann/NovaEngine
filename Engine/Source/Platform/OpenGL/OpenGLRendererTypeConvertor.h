#pragma once
#include "Rendering/RendererTypeConvertor.h"
#include "Runtime/Types.h"

typedef Nova::u32 GLenum;

namespace Nova
{
    struct OpenGLRendererTypeConvertor : RendererTypeConvertor<GLenum, GLenum, GLenum, GLenum, GLenum, GLenum,
    GLenum, GLenum, GLenum, GLenum, GLenum, GLenum>
    {
        GLenum ConvertCullMode(const CullMode Mode) const override;
        GLenum ConvertBlendFactor(const BlendFactor Factor) const override;
        GLenum ConvertBlendOperation(const BlendOperation Operation) const override;
        GLenum ConvertFilter(const Filter Filter) const override;
        GLenum ConvertFrontFace(const FrontFace Face) const override;
        GLenum ConvertPolygonMode(const PolygonMode Mode) const override;
        GLenum ConvertPrimitiveTopology(const PrimitiveTopology Topology) const override;
        GLenum ConvertSamplerAddressMode(const SamplerAddressMode Address) const override;
        GLenum ConvertShaderStage(const ShaderStageFlagBits Stage) const override;
        GLenum ConvertCompareOperation(const CompareOperation Operation) const override;
        GLenum ConvertFormat(const Format Format) const override;
        GLenum ConvertPresentMode(const PresentMode PresentMode) const override;
    };
}
