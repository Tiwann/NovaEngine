#pragma once
#include "Runtime/Format.h"
#include "Rendering/PresentMode.h"
#include "Rendering/ResourceState.h"
#include "Rendering/QueueType.h"
#include <dxgi1_6.h>
#include <directx/d3dx12.h>

#include "Rendering/SamplerAddressMode.h"
#include "Rendering/CompareOperation.h"
#include "Rendering/BlendFactor.h"
#include "Rendering/BlendOperation.h"
#include "Rendering/PolygonMode.h"
#include "Rendering/PrimitiveTopology.h"
#include "Rendering/VertexInputRate.h"


namespace Nova::D3D12
{
    template<typename OutType, typename InType>
    OutType Convert(const InType& value);

    template <>
    inline DXGI_FORMAT Convert(const Format& value)
    {
        switch (value)
        {
        case Format::None: return DXGI_FORMAT_UNKNOWN;

        case Format::R8_UNORM: return DXGI_FORMAT_R8_UNORM;
        case Format::R8_SNORM: return DXGI_FORMAT_R8_SNORM;
        case Format::R16_USHORT: return DXGI_FORMAT_R16_UNORM;
        case Format::R16_SHORT: return DXGI_FORMAT_R16_SNORM;
        case Format::R32_FLOAT: return DXGI_FORMAT_R32_FLOAT;
        case Format::R32_UINT: return DXGI_FORMAT_R32_UINT;
        case Format::R32_SINT: return DXGI_FORMAT_R32_SINT;

        case Format::R8G8_UNORM: return DXGI_FORMAT_R8G8_UNORM;
        case Format::R8G8_SNORM: return DXGI_FORMAT_R8G8_SNORM;
        case Format::R16G16_USHORT: return DXGI_FORMAT_R16G16_UNORM;
        case Format::R16G16_SHORT: return DXGI_FORMAT_R16G16_SNORM;
        case Format::R32G32_UINT: return DXGI_FORMAT_R32G32_UINT;
        case Format::R32G32_SINT: return DXGI_FORMAT_R32G32_SINT;
        case Format::R32G32_FLOAT: return DXGI_FORMAT_R32G32_FLOAT;

        case Format::R8G8B8_UNORM: return DXGI_FORMAT_UNKNOWN;
        case Format::R8G8B8_SNORM: return DXGI_FORMAT_UNKNOWN;
        case Format::R16G16B16_USHORT: return DXGI_FORMAT_UNKNOWN;
        case Format::R16G16B16_SHORT: return DXGI_FORMAT_UNKNOWN;
        case Format::R32G32B32_UINT: return DXGI_FORMAT_R32G32B32_UINT;
        case Format::R32G32B32_SINT: return DXGI_FORMAT_R32G32B32_SINT;
        case Format::R32G32B32_FLOAT: return DXGI_FORMAT_R32G32B32_FLOAT;

        case Format::R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case Format::R8G8B8A8_SNORM: return DXGI_FORMAT_R8G8B8A8_SNORM;
        case Format::R8G8B8A8_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case Format::R16G16B16A16_USHORT: return DXGI_FORMAT_R16G16B16A16_UNORM;
        case Format::R16G16B16A16_SHORT: return DXGI_FORMAT_R16G16B16A16_SNORM;
        case Format::R32G32B32A32_UINT: return DXGI_FORMAT_R32G32B32A32_UINT;
        case Format::R32G32B32A32_SINT: return DXGI_FORMAT_R32G32B32A32_SINT;
        case Format::R32G32B32A32_FLOAT: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case Format::R64G64B64A64_FLOAT: return DXGI_FORMAT_UNKNOWN;

        case Format::D32_FLOAT: return DXGI_FORMAT_D32_FLOAT;
        case Format::D32_FLOAT_S8_UINT: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        case Format::D24_UNORM_S8_UINT: return DXGI_FORMAT_D24_UNORM_S8_UINT;

        case Format::ASTC_4X4_UNORM:
        case Format::ASTC_4X4_FLOAT:
        case Format::ASTC_4X4_SRGB:
        case Format::ASTC_5X4_UNORM:
        case Format::ASTC_5X4_FLOAT:
        case Format::ASTC_5X4_SRGB:
        case Format::ASTC_5X5_UNORM:
        case Format::ASTC_5X5_FLOAT:
        case Format::ASTC_5X5_SRGB:
        case Format::ASTC_6X5_UNORM:
        case Format::ASTC_6X5_FLOAT:
        case Format::ASTC_6X5_SRGB:
        case Format::ASTC_6X6_UNORM:
        case Format::ASTC_6X6_FLOAT:
        case Format::ASTC_6X6_SRGB:
        case Format::ASTC_8X5_UNORM:
        case Format::ASTC_8X5_FLOAT:
        case Format::ASTC_8X5_SRGB:
        case Format::ASTC_8X6_UNORM:
        case Format::ASTC_8X6_FLOAT:
        case Format::ASTC_8X6_SRGB:
        case Format::ASTC_8X8_UNORM:
        case Format::ASTC_8X8_FLOAT:
        case Format::ASTC_8X8_SRGB:
        case Format::ASTC_10X5_UNORM:
        case Format::ASTC_10X5_FLOAT:
        case Format::ASTC_10X5_SRGB:
        case Format::ASTC_10X6_UNORM:
        case Format::ASTC_10X6_FLOAT:
        case Format::ASTC_10X6_SRGB:
        case Format::ASTC_10X8_UNORM:
        case Format::ASTC_10X8_FLOAT:
        case Format::ASTC_10X8_SRGB:
        case Format::ASTC_10X10_UNORM:
        case Format::ASTC_10X10_FLOAT:
        case Format::ASTC_10X10_SRGB:
        case Format::ASTC_12X10_UNORM:
        case Format::ASTC_12X10_FLOAT:
        case Format::ASTC_12X10_SRGB:
        case Format::ASTC_12X12_UNORM:
        case Format::ASTC_12X12_FLOAT:
        case Format::ASTC_12X12_SRGB: return DXGI_FORMAT_UNKNOWN;

        case Format::BC1_UNORM: return DXGI_FORMAT_BC1_UNORM;
        case Format::BC1_SRGB: return DXGI_FORMAT_BC1_UNORM_SRGB;
        case Format::BC2_UNORM: return DXGI_FORMAT_BC2_UNORM;
        case Format::BC2_SRGB: return DXGI_FORMAT_BC2_UNORM_SRGB;
        case Format::BC3_UNORM: return DXGI_FORMAT_BC3_UNORM;
        case Format::BC3_SRGB: return DXGI_FORMAT_BC3_UNORM_SRGB;
        case Format::BC4_UNORM: return DXGI_FORMAT_BC4_UNORM;
        case Format::BC4_SNORM: return DXGI_FORMAT_BC4_SNORM;
        case Format::BC5_UNORM: return DXGI_FORMAT_BC5_UNORM;
        case Format::BC5_SNORM: return DXGI_FORMAT_BC5_SNORM;
        case Format::BC7_UNORM: return DXGI_FORMAT_BC7_UNORM;
        case Format::BC7_SRGB: return DXGI_FORMAT_BC7_UNORM_SRGB;
        }

        return DXGI_FORMAT_UNKNOWN;
    }

    template <>
    inline DXGI_SWAP_EFFECT Convert(const PresentMode& value)
    {
        switch (value)
        {
        case PresentMode::Unknown: throw;
        case PresentMode::Immediate: return DXGI_SWAP_EFFECT_DISCARD;
        case PresentMode::Mailbox: return DXGI_SWAP_EFFECT_FLIP_DISCARD;
        case PresentMode::Fifo: return DXGI_SWAP_EFFECT_FLIP_DISCARD;
        default: throw;
        }
    }

    template<>
    inline D3D12_RESOURCE_STATES Convert(const ResourceState& resourceState)
    {
        switch (resourceState)
        {
        case ResourceState::Undefined: return D3D12_RESOURCE_STATE_COMMON;
        case ResourceState::General: return D3D12_RESOURCE_STATE_COMMON;
        case ResourceState::ShaderRead: return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
        case ResourceState::ShaderWrite: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        case ResourceState::ColorAttachment: return D3D12_RESOURCE_STATE_RENDER_TARGET;
        case ResourceState::DepthStencilAttachment: return D3D12_RESOURCE_STATE_DEPTH_WRITE;
        case ResourceState::TransferSource: return D3D12_RESOURCE_STATE_COPY_SOURCE;
        case ResourceState::TransferDest: return D3D12_RESOURCE_STATE_COPY_DEST;
        case ResourceState::Present: return D3D12_RESOURCE_STATE_PRESENT;
        default: return D3D12_RESOURCE_STATE_COMMON;
        }
    }

    template<>
    inline ResourceState Convert(const D3D12_RESOURCE_STATES& resourceState)
    {
        return ResourceState::Undefined;
    }

    template<>
    inline D3D12_COMMAND_LIST_TYPE Convert(const QueueType& queueType)
    {
        switch (queueType)
        {
        case QueueType::None: return D3D12_COMMAND_LIST_TYPE_NONE;
        case QueueType::Graphics: return D3D12_COMMAND_LIST_TYPE_DIRECT;
        case QueueType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
        case QueueType::Transfer: return D3D12_COMMAND_LIST_TYPE_COPY;
        default: return D3D12_COMMAND_LIST_TYPE_NONE;
        }
    }

    template<>
    inline QueueType Convert(const D3D12_COMMAND_LIST_TYPE& listType)
    {
        switch (listType)
        {
        case D3D12_COMMAND_LIST_TYPE_NONE: return QueueType::None;
        case D3D12_COMMAND_LIST_TYPE_DIRECT: return QueueType::Graphics;
        case D3D12_COMMAND_LIST_TYPE_COMPUTE: return QueueType::Compute;
        case D3D12_COMMAND_LIST_TYPE_COPY: return QueueType::Transfer;
        default: return QueueType::None;
        }
    }

    template<>
    inline D3D12_INPUT_CLASSIFICATION Convert(const VertexInputRate& rate)
    {
        switch (rate)
        {
        case VertexInputRate::Vertex: return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        case VertexInputRate::Instance: return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
        default: return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        }
    }

    template<>
    inline D3D12_PRIMITIVE_TOPOLOGY_TYPE Convert(const PrimitiveTopology& value)
    {
        switch (value)
        {
        case PrimitiveTopology::PointList: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
        case PrimitiveTopology::LineList: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        case PrimitiveTopology::LineStrip: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        case PrimitiveTopology::TriangleList: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        case PrimitiveTopology::TriangleStrip: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        case PrimitiveTopology::TriangleFan: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        default: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        }
    }

    template <>
    inline D3D12_BLEND Convert(const BlendFactor& factor)
    {
        switch (factor)
        {
        case BlendFactor::Zero: return D3D12_BLEND_ZERO;
        case BlendFactor::One: return D3D12_BLEND_ONE;
        case BlendFactor::SourceColor: return D3D12_BLEND_SRC_COLOR;
        case BlendFactor::OneMinusSourceColor: return D3D12_BLEND_INV_SRC_COLOR;
        case BlendFactor::DestColor: return D3D12_BLEND_DEST_COLOR;
        case BlendFactor::OneMinusDestColor: return D3D12_BLEND_INV_DEST_COLOR;
        case BlendFactor::SourceAlpha: return D3D12_BLEND_SRC_ALPHA;
        case BlendFactor::OneMinusSourceAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
        case BlendFactor::DestAlpha: return D3D12_BLEND_DEST_ALPHA;
        case BlendFactor::OneMinusDestAlpha: return D3D12_BLEND_INV_DEST_ALPHA;
        case BlendFactor::ConstantColor: return D3D12_BLEND_BLEND_FACTOR;
        case BlendFactor::OneMinusConstantColor: return D3D12_BLEND_INV_BLEND_FACTOR;
        case BlendFactor::ConstantAlpha: return D3D12_BLEND_BLEND_FACTOR;
        case BlendFactor::OneMinusConstantAlpha: return D3D12_BLEND_INV_BLEND_FACTOR;
        case BlendFactor::SourceAlphaSaturated: return D3D12_BLEND_SRC_ALPHA_SAT;
        case BlendFactor::Source1Color: return D3D12_BLEND_SRC1_COLOR;
        case BlendFactor::OneMinusSource1Color: return D3D12_BLEND_INV_SRC1_COLOR;
        case BlendFactor::Source1Alpha: return D3D12_BLEND_SRC1_ALPHA;
        case BlendFactor::OneMinusSource1Alpha: return D3D12_BLEND_INV_SRC1_ALPHA;
        default: return D3D12_BLEND_ZERO;
        }
    }

    template <>
    inline D3D12_BLEND_OP Convert(const BlendOperation& op)
    {
        switch (op)
        {
        case BlendOperation::Add: return D3D12_BLEND_OP_ADD;
        case BlendOperation::Min: return D3D12_BLEND_OP_MIN;
        case BlendOperation::Max: return D3D12_BLEND_OP_MAX;
        case BlendOperation::Subtract: return D3D12_BLEND_OP_SUBTRACT;
        case BlendOperation::ReverseSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
        default: return D3D12_BLEND_OP_ADD;
        }
    }

    template <>
    inline D3D12_COMPARISON_FUNC Convert(const CompareOperation& op)
    {
        switch (op)
        {
        case CompareOperation::Always: return D3D12_COMPARISON_FUNC_ALWAYS;
        case CompareOperation::Never: return D3D12_COMPARISON_FUNC_NEVER;
        case CompareOperation::Less: return D3D12_COMPARISON_FUNC_LESS;
        case CompareOperation::LessOrEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case CompareOperation::Equal: return D3D12_COMPARISON_FUNC_EQUAL;
        case CompareOperation::NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        case CompareOperation::Greater: return D3D12_COMPARISON_FUNC_GREATER;
        case CompareOperation::GreaterOrEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        default: return D3D12_COMPARISON_FUNC_ALWAYS;
        }
    }

    template <>
    inline D3D12_TEXTURE_ADDRESS_MODE Convert(const SamplerAddressMode& mode)
    {
        switch (mode)
        {
        case SamplerAddressMode::Repeat: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        case SamplerAddressMode::MirroredRepeat: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        case SamplerAddressMode::ClampToEdge: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        case SamplerAddressMode::ClampToBorder:return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        case SamplerAddressMode::MirrorClampToEdge:return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
        default: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        }
    }
}
