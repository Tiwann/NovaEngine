#pragma once
#include "BlendFactor.h"
#include "BlendOperation.h"
#include "CompareOperation.h"
#include "CullMode.h"
#include "Filter.h"
#include "FrontFace.h"
#include "PolygonMode.h"
#include "PresentMode.h"
#include "PrimitiveTopology.h"
#include "SamplerAddressMode.h"
#include "ShaderStage.h"
#include "Runtime/Format.h"

namespace Nova
{
    template<typename CullModeType,
    typename BlendFactorType,
    typename BlendOperationType,
    typename FilterType,
    typename FrontFaceType,
    typename PolygonModeType,
    typename PrimitiveTopologyType,
    typename SamplerAddressModeType,
    typename ShaderStageType,
    typename CompareOperationType,
    typename FormatType,
    typename PresentModeType>
    struct RendererTypeConvertor
    {
        virtual ~RendererTypeConvertor() = default;
        virtual CullModeType ConvertCullMode(const CullMode Mode) const = 0;
        virtual BlendFactorType ConvertBlendFactor(const BlendFactor Factor) const = 0;
        virtual BlendOperationType ConvertBlendOperation(const BlendOperation Operation) const = 0;
        virtual FilterType ConvertFilter(const Filter Filter) const = 0;
        virtual FrontFaceType ConvertFrontFace(const FrontFace Face) const = 0;
        virtual PolygonModeType ConvertPolygonMode(const PolygonMode Mode) const = 0;
        virtual PrimitiveTopologyType ConvertPrimitiveTopology(const PrimitiveTopology Topology) const = 0;
        virtual SamplerAddressModeType ConvertSamplerAddressMode(const SamplerAddressMode Address) const = 0;
        virtual ShaderStageType ConvertShaderStage(const ShaderStage Stage) const = 0;
        virtual CompareOperationType ConvertCompareOperation(const CompareOperation Operation) const = 0;
        virtual FormatType ConvertFormat(const Format Format) const = 0;
        virtual PresentModeType ConvertPresentMode(const PresentMode PresentMode) const = 0;
    };
}
