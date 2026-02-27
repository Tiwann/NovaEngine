#include "Containers/StringFormat.h"
#include "Shader.h"
#include "Buffer.h"
#include "DescriptorPool.h"
#include "RenderDevice.h"
#include "Sampler.h"
#include "Conversions.h"

#include <vulkan/vulkan.h>
#include <spirv_reflect.h>
#include <unordered_map>

#include "Runtime/Application.h"

namespace Nova::Vulkan
{
    static SlangCompileTarget GetCompileTarget(const ShaderTarget target)
    {
        switch (target)
        {
        case ShaderTarget::SPIRV: return SLANG_SPIRV;
        case ShaderTarget::GLSL: return SLANG_GLSL;
        case ShaderTarget::HLSL: return SLANG_HLSL;
        case ShaderTarget::DXBC: return SLANG_DXBC;
        case ShaderTarget::DXIL: return SLANG_DXIL;
        default: return SLANG_SPIRV;
        }
    }

    static ShaderStageFlagBits GetStage(const SlangStage stage)
    {
        switch (stage)
        {
        case SLANG_STAGE_NONE: return ShaderStageFlagBits::None;
        case SLANG_STAGE_VERTEX: return ShaderStageFlagBits::Vertex;
        case SLANG_STAGE_GEOMETRY: return ShaderStageFlagBits::Geometry;
        case SLANG_STAGE_FRAGMENT: return ShaderStageFlagBits::Fragment;
        case SLANG_STAGE_COMPUTE: return ShaderStageFlagBits::Compute;
        case SLANG_STAGE_RAY_GENERATION: return ShaderStageFlagBits::RayGeneration;
        case SLANG_STAGE_HULL: return ShaderStageFlagBits::Tessellation;
        case SLANG_STAGE_MESH: return ShaderStageFlagBits::Mesh;
        default: return ShaderStageFlagBits::None;
        }
    }

    static BindingType GetBindingType(const slang::BindingType bindingType)
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

    static StringView GetErrorString(const Slang::ComPtr<slang::IBlob>& blob)
    {
        const StringView errorString = { (const char*)blob->getBufferPointer(), blob->getBufferSize() };
        return errorString;
    };

    bool Shader::Initialize(const ShaderCreateInfo& createInfo)
    {
        // TODO: /!\ MEMORY LEAK HERE
        m_EntryPoints.Clear();
        m_ShaderModules.Clear();
        m_BindingSetLayouts.Clear();

        Application& application = Application::GetCurrentApplication();
        slang::IGlobalSession* slangSession = application.GetSlangSession();

        slang::TargetDesc shaderTargetDesc;
        shaderTargetDesc.format = GetCompileTarget(createInfo.target);
        shaderTargetDesc.floatingPointMode = SLANG_FLOATING_POINT_MODE_DEFAULT;
        shaderTargetDesc.lineDirectiveMode = SLANG_LINE_DIRECTIVE_MODE_DEFAULT;
        shaderTargetDesc.profile = slangSession->findProfile("spirv_1_5");

        slang::CompilerOptionEntry entries[] = {
            {slang::CompilerOptionName::MinimumSlangOptimization, slang::CompilerOptionValue(slang::CompilerOptionValueKind::Int, 1)},
            {slang::CompilerOptionName::Optimization, slang::CompilerOptionValue(slang::CompilerOptionValueKind::Int, SLANG_OPTIMIZATION_LEVEL_NONE)},
        };
        slang::SessionDesc sessionDesc;
        sessionDesc.targets = &shaderTargetDesc;
        sessionDesc.targetCount = 1;
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;
        sessionDesc.searchPaths = createInfo.includes.Transform<const char*>([](const StringView includeDir) { return *includeDir; }).Data();
        sessionDesc.searchPathCount = createInfo.includes.Count();
        sessionDesc.compilerOptionEntries = entries;
        sessionDesc.compilerOptionEntryCount = std::size(entries);

        SlangResult result = slangSession->createSession(sessionDesc, m_Session.writeRef());
        if (SLANG_FAILED(result)) return false;

        Slang::ComPtr<slang::IBlob> errorBlob = nullptr;

        m_Module = Slang::ComPtr(m_Session->loadModuleFromSource(
            *createInfo.moduleInfo.name,
            *createInfo.moduleInfo.filepath,
            nullptr,
            errorBlob.writeRef()));

        if (!m_Module)
        {
            std::println(std::cerr, "Failed to load slang module [{}]: {}", *createInfo.moduleInfo.name, *GetErrorString(errorBlob));
            return false;
        }

        for (const ShaderEntryPoint& shaderEntryPoint : createInfo.entryPoints)
        {
            Slang::ComPtr<slang::IEntryPoint> entryPoint = nullptr;
            result = m_Module->findEntryPointByName(*shaderEntryPoint.name, entryPoint.writeRef());
            if (SLANG_FAILED(result))
            {
                std::println(std::cerr, "Entry point '{}' not found. Compilation failed.", *shaderEntryPoint.name);
                return false;
            }
            m_EntryPoints.Add(entryPoint);
        }

        Array<slang::IComponentType*> entryPoints = m_EntryPoints.Transform<slang::IComponentType*>(
            [](const Slang::ComPtr<slang::IEntryPoint>& entryPoint) { return entryPoint; });

        result = m_Session->createCompositeComponentType(entryPoints.Data(), entryPoints.Count(), m_Program.writeRef(), errorBlob.writeRef());
        if (SLANG_FAILED(result))
        {
            std::println(std::cerr, "Failed to create shader program: {}", *GetErrorString(errorBlob));
            return false;
        }

        result = m_Program->link(m_LinkedProgram.writeRef(), errorBlob.writeRef());
        if (SLANG_FAILED(result))
        {
            std::println(std::cerr, "Failed to link shader program: {}", *GetErrorString(errorBlob));
            return false;
        }


        Array<SpvReflectShaderModule> reflectModules;

        for (size_t entryPointIndex = 0; entryPointIndex < m_EntryPoints.Count(); ++entryPointIndex)
        {
            Slang::ComPtr<slang::IBlob> entryPointCode = nullptr;
            result = m_LinkedProgram->getEntryPointCode(entryPointIndex, 0, entryPointCode.writeRef(), errorBlob.writeRef());
            if (SLANG_FAILED(result))
            {
                std::println(std::cerr, "Failed to get entry point code: {}", *GetErrorString(errorBlob));
                return false;
            }

            const ShaderStageFlagBits& shaderStage = createInfo.entryPoints[entryPointIndex].stage;

            ShaderModuleCreateInfo shaderModuleCreateInfo;
            shaderModuleCreateInfo.code = (const uint32_t*)entryPointCode->getBufferPointer();
            shaderModuleCreateInfo.codeSize = entryPointCode->getBufferSize();
            shaderModuleCreateInfo.device = createInfo.device;
            shaderModuleCreateInfo.stage = shaderStage;

            ShaderModule shaderModule;
            shaderModule.Initialize(shaderModuleCreateInfo);
            m_ShaderModules.Add(shaderModule);

            SpvReflectShaderModule reflectModule;
            spvReflectCreateShaderModule(entryPointCode->getBufferSize(), entryPointCode->getBufferPointer(), &reflectModule);
            reflectModules.Add(reflectModule);
        }

        slang::ProgramLayout* programLayout = m_LinkedProgram->getLayout();
        slang::VariableLayoutReflection* globalsVarLayout = programLayout->getGlobalParamsVarLayout();
        slang::TypeLayoutReflection* globalsTypeLayout = globalsVarLayout->getTypeLayout();

        ShaderStageFlags stageFlags = ShaderStageFlagBits::None;
        for (int e = 0; e < programLayout->getEntryPointCount(); ++e)
        {
            slang::EntryPointReflection* ep = programLayout->getEntryPointByIndex(e);
            stageFlags |= GetStage(ep->getStage());
        }

        Array<ShaderPushConstantRange> ranges;
        const auto GetBindingType = [](const SpvReflectDescriptorType type)
        {
            switch (type)
            {
            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER: return BindingType::Sampler;
            case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: return BindingType::CombinedTextureSampler;
            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE: return BindingType::SampledTexture;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE: return BindingType::StorageTexture;
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER: return BindingType::UniformTexelBuffer;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER: return BindingType::StorageTexelBuffer;
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return BindingType::UniformBuffer;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER: return BindingType::StorageBuffer;
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: return BindingType::InlineUniformBlock;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC: return BindingType::InlineUniformBlock;
            case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: return BindingType::InputAttachment;
            case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR: return BindingType::AccelerationStructure;
            }
            return (BindingType)0;
        };

        const auto GetShaderStage = [](SpvReflectShaderStageFlagBits bits)
        {
            switch (bits)
            {
            case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT: return ShaderStageFlagBits::Vertex;
            case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT: return ShaderStageFlagBits::Tessellation;
            case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT: return ShaderStageFlagBits::None;
            case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT: return ShaderStageFlagBits::Geometry;
            case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT: return ShaderStageFlagBits::Fragment;
            case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT: return ShaderStageFlagBits::Compute;
            case SPV_REFLECT_SHADER_STAGE_TASK_BIT_NV: return ShaderStageFlagBits::None;
            case SPV_REFLECT_SHADER_STAGE_MESH_BIT_NV: return ShaderStageFlagBits::Mesh;
            case SPV_REFLECT_SHADER_STAGE_RAYGEN_BIT_KHR: return ShaderStageFlagBits::RayGeneration;
            case SPV_REFLECT_SHADER_STAGE_ANY_HIT_BIT_KHR: return ShaderStageFlagBits::None;
            case SPV_REFLECT_SHADER_STAGE_CLOSEST_HIT_BIT_KHR: return ShaderStageFlagBits::None;
            case SPV_REFLECT_SHADER_STAGE_MISS_BIT_KHR: return ShaderStageFlagBits::None;
            case SPV_REFLECT_SHADER_STAGE_INTERSECTION_BIT_KHR: return ShaderStageFlagBits::None;
            case SPV_REFLECT_SHADER_STAGE_CALLABLE_BIT_KHR: return ShaderStageFlagBits::None;
            default: return ShaderStageFlagBits::None;
            }
        };

        for (size_t moduleIndex = 0; moduleIndex < reflectModules.Count(); moduleIndex++)
        {
            const SpvReflectShaderModule* reflectModule = &reflectModules[moduleIndex];
            uint32_t pushConstantBlockCount = 0;
            spvReflectEnumeratePushConstantBlocks(reflectModule, &pushConstantBlockCount, nullptr);
            Array<SpvReflectBlockVariable*> pushConstantBlocks(pushConstantBlockCount);
            spvReflectEnumeratePushConstantBlocks(reflectModule, &pushConstantBlockCount, pushConstantBlocks.Data());

            for (SpvReflectBlockVariable* block : pushConstantBlocks)
            {
                ShaderPushConstantRange range;
                range.offset = block->offset;
                range.size = block->size;
                range.stageFlags = GetShaderStage(reflectModule->shader_stage);
                ranges.Add(range);
            }


            uint32_t spvReflectSetCount = 0;
            spvReflectEnumerateDescriptorSets(reflectModule, &spvReflectSetCount, nullptr);
            Array<SpvReflectDescriptorSet*> sets(spvReflectSetCount);
            spvReflectEnumerateDescriptorSets(reflectModule, &spvReflectSetCount, sets.Data());

            for (const SpvReflectDescriptorSet* set : sets)
            {
                Ref<ShaderBindingSetLayout>* bindingSetLayout = m_BindingSetLayouts.Single([&set](const Ref<ShaderBindingSetLayout>& layout) { return layout->GetSetIndex() == set->set; });
                if (bindingSetLayout)
                {
                    for (size_t bindingIndex = 0; bindingIndex < set->binding_count; ++bindingIndex)
                    {
                        SpvReflectDescriptorBinding* binding = set->bindings[bindingIndex];
                        (*bindingSetLayout)->SetBinding(binding->binding, {binding->name, stageFlags, GetBindingType(binding->descriptor_type), binding->count });
                    }
                } else
                {
                    Ref<ShaderBindingSetLayout> newBindingSetLayout = new ShaderBindingSetLayout();
                    newBindingSetLayout->Initialize(createInfo.device, set->set);
                    for (size_t bindingIndex = 0; bindingIndex < set->binding_count; ++bindingIndex)
                    {
                        SpvReflectDescriptorBinding* binding = set->bindings[bindingIndex];
                        newBindingSetLayout->SetBinding(binding->binding, {binding->name, stageFlags, GetBindingType(binding->descriptor_type), binding->count });
                    }
                    m_BindingSetLayouts.Emplace(Memory::Move(newBindingSetLayout));
                }
            }
        }

        for (Ref<ShaderBindingSetLayout>& setLayout : m_BindingSetLayouts)
        {
            if (setLayout->BindingCount() > 0)
            {
                setLayout->Build();
            }
        }


        for (SpvReflectShaderModule& reflectModule : reflectModules)
            spvReflectDestroyShaderModule(&reflectModule);

        RenderDevice* device = (RenderDevice*)createInfo.device;

        Array<VkDescriptorSetLayout> descriptorSetLayouts = m_BindingSetLayouts.Transform<VkDescriptorSetLayout>(
            [](const Ref<ShaderBindingSetLayout>& setLayout)
        {
            return setLayout->GetHandle();
        });

        Array<VkPushConstantRange> pushConstantRanges = ranges.Transform<VkPushConstantRange>([](const ShaderPushConstantRange& range)
        {
            VkPushConstantRange pcRange;
            pcRange.offset = range.offset;
            pcRange.size = range.size;
            pcRange.stageFlags = Convert<VkShaderStageFlags>(range.stageFlags);
            return pcRange;
        });

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutCreateInfo.setLayoutCount = descriptorSetLayouts.Count();
        pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.Data();
        pipelineLayoutCreateInfo.pushConstantRangeCount = ranges.Count();
        pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.Data();

        vkDestroyPipelineLayout(device->GetHandle(), m_PipelineLayout, nullptr);
        vkCreatePipelineLayout(device->GetHandle(), &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout);

        m_Device = device;
        return true;
    }

    void Shader::Destroy()
    {
        for (Ref<ShaderBindingSetLayout>& setLayout : m_BindingSetLayouts)
            setLayout->Destroy();


        for (auto& shaderModule : m_ShaderModules)
            shaderModule.Destroy();

        if (!m_Device) return;
        vkDestroyPipelineLayout(m_Device->GetHandle(), m_PipelineLayout, nullptr);
    }

    Ref<Nova::ShaderBindingSet> Shader::CreateBindingSet(const size_t setIndex) const
    {
        const Ref<ShaderBindingSetLayout>* setLayout = m_BindingSetLayouts.Single([&setIndex](const Ref<ShaderBindingSetLayout>& setLayout) { return setLayout->GetSetIndex() == (uint32_t)setIndex; });
        if (!setLayout) return nullptr;

        ShaderBindingSetCreateInfo createInfo;
        createInfo.device = (Nova::RenderDevice*)m_Device;
        createInfo.pool = m_Device->GetDescriptorPool();
        createInfo.layout = *setLayout;

        ShaderBindingSet* bindingSet = new ShaderBindingSet();
        if (!bindingSet->Initialize(createInfo))
            return nullptr;

        return Ref(bindingSet);
    }

    Array<Ref<Nova::ShaderBindingSet>> Shader::CreateBindingSets() const
    {
        if (!m_Device) return {};

        Array<Ref<Nova::ShaderBindingSet>> bindingSets;

        for (const auto& setLayout : m_BindingSetLayouts)
        {
            ShaderBindingSetCreateInfo createInfo;
            createInfo.device = (Nova::RenderDevice*)m_Device;
            createInfo.pool = m_Device->GetDescriptorPool();
            createInfo.layout = setLayout;

            ShaderBindingSet* bindingSet = new ShaderBindingSet();
            bindingSet->Initialize(createInfo);

            bindingSets.Add(Ref(bindingSet));
        }

        return bindingSets;
    }

    const Array<ShaderModule>& Shader::GetShaderModules() const
    {
        return m_ShaderModules;
    }

    const ShaderModule& Shader::GetShaderModule(const ShaderStageFlagBits stage) const
    {
        for (const auto& shaderModule : m_ShaderModules)
            if (shaderModule.GetStage() == stage)
                return shaderModule;
        throw;
    }

    const Array<Ref<ShaderBindingSetLayout>>& Shader::GetBindingSetLayouts() const
    {
        return m_BindingSetLayouts;
    }

    ShaderStageFlags Shader::GetShaderStageFlags() const
    {
        ShaderStageFlags stageFlags = ShaderStageFlagBits::None;
        for (const auto& shaderModule : m_ShaderModules)
            stageFlags |= shaderModule.GetStage();
        return stageFlags;
    }

    VkPipelineLayout Shader::GetPipelineLayout() const
    {
        return m_PipelineLayout;
    }

    Array<VkDescriptorSetLayout> Shader::GetDescriptorSetLayouts() const
    {
        return m_BindingSetLayouts.Transform<VkDescriptorSetLayout>(
            [](const Ref<ShaderBindingSetLayout>& bindingSetLayout)
            {
                return bindingSetLayout->GetHandle();
            });
    }
}
