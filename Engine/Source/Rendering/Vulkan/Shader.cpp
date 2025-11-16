#include "Containers/StringFormat.h"
#include "Shader.h"
#include "Buffer.h"
#include "DescriptorPool.h"
#include "Device.h"
#include "Sampler.h"
#include "Conversions.h"

#include <vulkan/vulkan.h>
#include <spirv_reflect.h>

namespace Nova::Vulkan
{
    static SlangCompileTarget GetCompileTarget(const Rendering::ShaderTarget target)
    {
        switch (target)
        {
        case Rendering::ShaderTarget::SPIRV: return SLANG_SPIRV;
        case Rendering::ShaderTarget::GLSL: return SLANG_GLSL;
        case Rendering::ShaderTarget::HLSL: return SLANG_HLSL;
        case Rendering::ShaderTarget::DXBC: return SLANG_DXBC;
        case Rendering::ShaderTarget::DXIL: return SLANG_DXIL;
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

    bool Shader::Initialize(const Rendering::ShaderCreateInfo& createInfo)
    {
        // TODO: /!\ MEMORY LEAK HERE
        m_EntryPoints.Clear();
        m_ShaderModules.Clear();
        m_BindingSetLayouts.Clear();



        slang::TargetDesc shaderTargetDesc;
        shaderTargetDesc.format = GetCompileTarget(createInfo.target);
        shaderTargetDesc.floatingPointMode = SLANG_FLOATING_POINT_MODE_DEFAULT;
        shaderTargetDesc.lineDirectiveMode = SLANG_LINE_DIRECTIVE_MODE_DEFAULT;
        shaderTargetDesc.profile = createInfo.slang->findProfile("spirv_1_5");

        slang::SessionDesc sessionDesc;
        sessionDesc.targets = &shaderTargetDesc;
        sessionDesc.targetCount = 1;
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;
        sessionDesc.searchPaths = createInfo.includes.Transform<const char*>([](const StringView includeDir) { return *includeDir; }).Data();
        sessionDesc.searchPathCount = createInfo.includes.Count();

        SlangResult result = createInfo.slang->createSession(sessionDesc, m_Session.writeRef());
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

        for (const Rendering::ShaderEntryPoint& shaderEntryPoint : createInfo.entryPoints)
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

            Rendering::ShaderModuleCreateInfo shaderModuleCreateInfo;
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

        Array<Rendering::ShaderPushConstantRange> ranges;
        uint32_t paramCount = programLayout->getParameterCount();
        uint32_t offset = 0;
        for (uint32_t paramIndex = 0; paramIndex < paramCount; ++paramIndex)
        {
            slang::VariableLayoutReflection* variableLayout = programLayout->getParameterByIndex(paramIndex);
            slang::VariableReflection* variable = variableLayout->getVariable();
            slang::Attribute* attribute = variable->findAttributeByName(createInfo.slang, "vk_push_constant");
            if (!attribute) continue;

            slang::TypeLayoutReflection* typeLayout = variableLayout->getTypeLayout();
            size_t size = typeLayout->getElementTypeLayout()->getSize();

            if (size > 0)
            {
                Rendering::ShaderPushConstantRange range;
                range.offset = offset;
                range.size = size;
                range.stageFlags = stageFlags;
                ranges.Add(range);
                offset += size;
            }
        }


        const uint32_t setCount = globalsTypeLayout->getDescriptorSetCount();
        for (uint32_t setIndex = 0; setIndex < setCount; ++setIndex)
        {
            uint32_t bindingCount = globalsTypeLayout->getDescriptorSetDescriptorRangeCount(setIndex);

            ShaderBindingSetLayout bindingSetLayout;
            for (uint32_t bindingIndex = 0; bindingIndex < bindingCount; ++bindingIndex)
            {
                const BindingType bindingType = GetBindingType(globalsTypeLayout->getDescriptorSetDescriptorRangeType(setIndex, bindingIndex));
                if (bindingType == BindingType::PushConstant) continue;

                size_t bindingIndexOffset = globalsTypeLayout->getDescriptorSetDescriptorRangeIndexOffset(setIndex, bindingIndex);
                const uint32_t descriptorCount = globalsTypeLayout->getDescriptorSetDescriptorRangeDescriptorCount(setIndex, bindingIndex);

                const char* bindingName = nullptr;

                for (const SpvReflectShaderModule& reflectModule : reflectModules)
                {
                    if (bindingName) break;

                    uint32_t spvReflectBindingCount = 0;
                    spvReflectEnumerateDescriptorBindings(&reflectModule, &spvReflectBindingCount, nullptr);
                    Array<SpvReflectDescriptorBinding*> bindings(spvReflectBindingCount);
                    spvReflectEnumerateDescriptorBindings(&reflectModule, &spvReflectBindingCount, bindings.Data());

                    for (const SpvReflectDescriptorBinding* binding : bindings)
                    {
                        if (binding->set == setIndex && binding->binding == bindingIndex)
                        {
                            bindingName = binding->name;
                            break;
                        }
                    }
                }

                bindingSetLayout.SetBinding(bindingIndexOffset, { bindingName, stageFlags, bindingType, descriptorCount });
            }

            if (bindingSetLayout.BindingCount() > 0)
            {
                bindingSetLayout.Initialize(createInfo.device, setIndex);
                bindingSetLayout.Build();
                m_BindingSetLayouts.Add(bindingSetLayout);
            }
        }

        for (SpvReflectShaderModule& reflectModule : reflectModules)
            spvReflectDestroyShaderModule(&reflectModule);

        Device* device = (Device*)createInfo.device;

        Array<VkDescriptorSetLayout> descriptorSetLayouts = m_BindingSetLayouts.Transform<VkDescriptorSetLayout>(
            [](const ShaderBindingSetLayout& setLayout)
        {
            return setLayout.GetHandle();
        });

        Array<VkPushConstantRange> pushConstantRanges = ranges.Transform<VkPushConstantRange>([](const Rendering::ShaderPushConstantRange& range)
        {
            VkPushConstantRange pcRange;
            pcRange.offset = range.offset;
            pcRange.size = range.size;
            pcRange.stageFlags = Convert<ShaderStageFlags, VkShaderStageFlags>(range.stageFlags);
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
        for (auto& bindingSetLayout : m_BindingSetLayouts)
            bindingSetLayout.Destroy();

        for (auto& shaderModule : m_ShaderModules)
            shaderModule.Destroy();

        vkDestroyPipelineLayout(m_Device->GetHandle(), m_PipelineLayout, nullptr);
    }

    Ref<Rendering::ShaderBindingSet> Shader::CreateBindingSet(const size_t setIndex) const
    {
        Rendering::ShaderBindingSetCreateInfo createInfo;
        createInfo.device = m_Device;
        createInfo.pool = m_Device->GetDescriptorPool();
        createInfo.layout = &m_BindingSetLayouts[setIndex];

        ShaderBindingSet* bindingSet = new ShaderBindingSet();
        if (!bindingSet->Initialize(createInfo))
            return nullptr;

        return Ref(bindingSet);
    }

    Array<Ref<Rendering::ShaderBindingSet>> Shader::CreateBindingSets() const
    {
        Array<Ref<Rendering::ShaderBindingSet>> bindingSets;

        for (const auto& setLayout : m_BindingSetLayouts)
        {
            Rendering::ShaderBindingSetCreateInfo createInfo;
            createInfo.device = m_Device;
            createInfo.pool = m_Device->GetDescriptorPool();
            createInfo.layout = &setLayout;

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

    const Array<ShaderBindingSetLayout>& Shader::GetBindingSetLayouts() const
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
            [](const ShaderBindingSetLayout& bindingSetLayout)
            {
                return bindingSetLayout.GetHandle();
            });
    }
}
