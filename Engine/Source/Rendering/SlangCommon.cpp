#include "SlangCommon.h"

namespace Nova
{
    SlangCompileTarget GetCompileTarget(const ShaderCompileTarget target, const RenderDeviceType deviceType)
    {

        switch (target)
        {
        case ShaderCompileTarget::SPIRV: return SLANG_SPIRV;
        case ShaderCompileTarget::GLSL: return SLANG_GLSL;
        case ShaderCompileTarget::HLSL: return SLANG_HLSL;
        case ShaderCompileTarget::DXBC: return SLANG_DXBC;
        case ShaderCompileTarget::DXIL: return SLANG_DXIL;
        case ShaderCompileTarget::AUTO:
            {
                switch (deviceType)
                {
                case RenderDeviceType::Null: return SLANG_TARGET_NONE;
#ifdef NOVA_HAS_VULKAN
                case RenderDeviceType::Vulkan: return SLANG_SPIRV;
#endif
#ifdef NOVA_HAS_D3D12
                case RenderDeviceType::D3D12: return SLANG_DXIL;
#endif
#ifdef NOVA_HAS_OPENGL
#ifdef NOVA_OPENGL_USES_SPIRV
                case RenderDeviceType::OpenGL: return SLANG_SPIRV;
#else
                case RenderDeviceType::OpenGL: return SLANG_GLSL;
#endif
#endif
                default: return SLANG_TARGET_NONE;
                }
            }
        default: return SLANG_TARGET_UNKNOWN;
        }
    }

    ShaderStageFlagBits GetStage(const SlangStage stage)
    {
        switch (stage)
        {
        case SLANG_STAGE_NONE: return ShaderStageFlagBits::None;
        case SLANG_STAGE_VERTEX: return ShaderStageFlagBits::Vertex;
        case SLANG_STAGE_GEOMETRY: return ShaderStageFlagBits::Geometry;
        case SLANG_STAGE_FRAGMENT: return ShaderStageFlagBits::Fragment;
        case SLANG_STAGE_COMPUTE: return ShaderStageFlagBits::Compute;
        case SLANG_STAGE_RAY_GENERATION: return ShaderStageFlagBits::RayGeneration;
        case SLANG_STAGE_HULL: return ShaderStageFlagBits::TessellationControl;
        case SLANG_STAGE_MESH: return ShaderStageFlagBits::Mesh;
        default: return ShaderStageFlagBits::None;
        }
    }

    BindingType GetBindingType(const slang::BindingType bindingType)
    {
        switch (bindingType) {
        case slang::BindingType::Sampler: return BindingType::Sampler;
        case slang::BindingType::Texture: return BindingType::SampledTexture;
        case slang::BindingType::ConstantBuffer:
        case slang::BindingType::ParameterBlock: return BindingType::UniformBuffer;
        case slang::BindingType::TypedBuffer: return BindingType::UniformTexelBuffer;
        case slang::BindingType::RawBuffer: return BindingType::UniformBuffer;
        case slang::BindingType::CombinedTextureSampler: return BindingType::CombinedTextureSampler;
        case slang::BindingType::InputRenderTarget: return BindingType::InputAttachment;
        case slang::BindingType::InlineUniformData: return BindingType::InlineUniformBlock;
        case slang::BindingType::RayTracingAccelerationStructure: return BindingType::AccelerationStructure;
        case slang::BindingType::VaryingInput:
            break;
        case slang::BindingType::VaryingOutput:
            break;
        case slang::BindingType::ExistentialValue:
            break;
        case slang::BindingType::PushConstant: return BindingType::PushConstant;
        case slang::BindingType::MutableFlag:
            break;
        case slang::BindingType::MutableTexture: return BindingType::StorageTexture;
        case slang::BindingType::MutableTypedBuffer: return BindingType::StorageBuffer;
        case slang::BindingType::MutableRawBuffer: return BindingType::StorageBuffer;
        case slang::BindingType::BaseMask:
            break;
        case slang::BindingType::ExtMask:
            break;
        default: ;
        }
        return BindingType::PushConstant;
    }

    StringView GetErrorString(const Slang::ComPtr<slang::IBlob>& blob)
    {
        const StringView errorString = { (const char*)blob->getBufferPointer(), blob->getBufferSize() };
        return errorString;
    }
}
