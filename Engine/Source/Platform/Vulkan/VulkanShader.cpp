#include "VulkanShader.h"
#include "VulkanRenderer.h"
#include "VulkanCommandBuffer.h"
#include "Runtime/Application.h"
#include "Runtime/Log.h"
#include <slang/slang.h>


namespace Nova
{
    static slang::IBlob* Diagnostic = nullptr;

    VulkanShader::VulkanShader(Renderer* Renderer, const String& Name, const Path& Filepath) : Shader(Renderer, Name, Filepath)
    {
        slang::IGlobalSession* Slang = m_Renderer->GetOwner()->GetSlangSession();
        slang::TargetDesc TargetDesc = {};
        TargetDesc.format = SLANG_SPIRV;
        TargetDesc.floatingPointMode = SLANG_FLOATING_POINT_MODE_DEFAULT;


        const Path ShaderDirs = Renderer->GetOwner()->GetEngineAssetsDirectory() / "Shaders";
        const String ShaderIncludeDirectory = ShaderDirs.string().c_str();
        Array<const char*> IncludeDirectories;
        IncludeDirectories.Add(*ShaderIncludeDirectory);


        slang::SessionDesc SessionDesc = {};
        SessionDesc.targetCount = 1;
        SessionDesc.targets = &TargetDesc;
        SessionDesc.searchPaths = IncludeDirectories.Data();
        SessionDesc.searchPathCount = IncludeDirectories.Count();
        SessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;

        if (SLANG_FAILED(Slang->createSession(SessionDesc, &m_Compiler)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to create Slang session!");
        }
    }

    VulkanShader::~VulkanShader()
    {
        const VulkanRenderer* Renderer = dynamic_cast<VulkanRenderer*>(m_Renderer);
        const VkDevice Device = Renderer->GetDevice();
        for (VulkanShaderModule& Module : m_ShaderModules)
        {
            Module.Stage = ShaderStageFlagBits::None;
            Module.CompiledCode->release();
            Module.EntryPoint->release();
            vkDestroyShaderModule(Device, Module.Handle, nullptr);
        }

        if (!m_DescriptorSets.IsEmpty())
        {
            vkFreeDescriptorSets(Device, Renderer->GetDescriptorPool(), m_DescriptorSets.Count(), m_DescriptorSets.Data());
        }

        vkDestroyPipelineLayout(Device, m_PipelineLayout, nullptr);
        for (const VkDescriptorSetLayout& Layout : m_DescriptorSetLayouts)
            vkDestroyDescriptorSetLayout(Device, Layout, nullptr);
    }

    bool VulkanShader::Compile()
    {
        if (!m_Compiler)
            return false;


        m_ShaderModule = m_Compiler->loadModuleFromSource(*m_Name, m_Filepath.string().c_str(), nullptr, &Diagnostic);
        if (!m_ShaderModule)
        {
            NOVA_LOG(Shader, Verbosity::Error, "Failed to compile shader: Failed to load module");
            NOVA_LOG(Shader, Verbosity::Error, "{}", StringView((const char*)Diagnostic->getBufferPointer(), Diagnostic->getBufferSize()));
            return false;
        }

        FindShaderStage("VertexMain", ShaderStageFlagBits::Vertex);
        FindShaderStage("GeometryMain", ShaderStageFlagBits::Geometry);
        FindShaderStage("FragmentMain", ShaderStageFlagBits::Fragment);

        if (m_ShaderModules.IsEmpty())
        {
            NOVA_LOG(Shader, Verbosity::Error, "No shader stage found!");
            return false;
        }
        return true;
    }

    bool VulkanShader::Link()
    {
        if (!m_Compiler) return false;

        Array<slang::IComponentType*> EntryPoints;
        for (const VulkanShaderModule& Module : m_ShaderModules)
            EntryPoints.Add(Module.EntryPoint);

        if (SLANG_FAILED(m_Compiler->createCompositeComponentType(EntryPoints.Data(), EntryPoints.Count(), &m_Program, &Diagnostic)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to create program!");
            NOVA_LOG(Shader, Verbosity::Error, "{}", StringView((const char*)Diagnostic->getBufferPointer(), Diagnostic->getBufferSize()));
            return false;
        }

        if (SLANG_FAILED(m_Program->link(&m_LinkedProgram, &Diagnostic)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to link program!");
            NOVA_LOG(Shader, Verbosity::Error, "{}", StringView((const char*)Diagnostic->getBufferPointer(), Diagnostic->getBufferSize()));
            return false;
        }
        return true;
    }

    static VkDescriptorType SlangConvertTypeReflectionKind(const slang::BindingType Kind)
    {
        switch (Kind)
        {
        case slang::BindingType::Unknown: throw;
        case slang::BindingType::Sampler: return VK_DESCRIPTOR_TYPE_SAMPLER;
        case slang::BindingType::Texture: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case slang::BindingType::ConstantBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case slang::BindingType::ParameterBlock: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case slang::BindingType::TypedBuffer: throw;
        case slang::BindingType::RawBuffer: throw;
        case slang::BindingType::CombinedTextureSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case slang::BindingType::InputRenderTarget: return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        case slang::BindingType::InlineUniformData: return VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK;
        case slang::BindingType::RayTracingAccelerationStructure: return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
        case slang::BindingType::VaryingInput: throw;
        case slang::BindingType::VaryingOutput: throw;
        case slang::BindingType::ExistentialValue: throw;
        case slang::BindingType::PushConstant: break;
        case slang::BindingType::MutableFlag: throw;
        case slang::BindingType::MutableTexture: throw;
        case slang::BindingType::MutableTypedBuffer: throw;
        case slang::BindingType::MutableRawBuffer: throw;
        case slang::BindingType::BaseMask: throw;
        case slang::BindingType::ExtMask: throw;
        default: throw;
        }
    }

    static VkShaderStageFlagBits SlangConvertShaderStage(const SlangStage Stage)
    {
        switch (Stage) {
        case SLANG_STAGE_NONE: throw;
        case SLANG_STAGE_VERTEX: return VK_SHADER_STAGE_VERTEX_BIT;
        case SLANG_STAGE_FRAGMENT: return VK_SHADER_STAGE_FRAGMENT_BIT;
        case SLANG_STAGE_COMPUTE: return VK_SHADER_STAGE_COMPUTE_BIT;
        case SLANG_STAGE_RAY_GENERATION: return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
        case SLANG_STAGE_INTERSECTION: return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
        case SLANG_STAGE_ANY_HIT: return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
        case SLANG_STAGE_CLOSEST_HIT: return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        case SLANG_STAGE_MISS: return VK_SHADER_STAGE_MISS_BIT_KHR;
        case SLANG_STAGE_CALLABLE: return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
        case SLANG_STAGE_MESH: return VK_SHADER_STAGE_MESH_BIT_EXT;
        default: throw;
        }

    }

    bool VulkanShader::Validate()
    {
        if (!m_LinkedProgram) return false;

        const VulkanRenderer* Renderer = dynamic_cast<VulkanRenderer*>(m_Renderer);
        const VkDevice Device = Renderer->GetDevice();

        for (size_t i = 0; i < m_ShaderModules.Count(); i++)
        {
            if (SLANG_FAILED(m_LinkedProgram->getEntryPointCode(i, 0, &m_ShaderModules[i].CompiledCode, &Diagnostic)))
            {
                NOVA_LOG(Application, Verbosity::Error, "Failed to get entry point code!");
                NOVA_LOG(Shader, Verbosity::Error, "{}", StringView((const char*)Diagnostic->getBufferPointer(), Diagnostic->getBufferSize()));
                return false;
            }
        }


        for (VulkanShaderModule& Module : m_ShaderModules)
        {
            VkShaderModuleCreateInfo ModuleCreateInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
            ModuleCreateInfo.codeSize = Module.CompiledCode->getBufferSize();
            ModuleCreateInfo.pCode = (const u32*)Module.CompiledCode->getBufferPointer();

            if (VK_FAILED(vkCreateShaderModule(Device, &ModuleCreateInfo, nullptr, &Module.Handle)))
            {
                NOVA_VULKAN_ERROR("Failed to create Vulkan shader module!");
                return false;
            }
        }


        slang::ProgramLayout* Layout = m_LinkedProgram->getLayout();
        slang::VariableLayoutReflection* Globals = Layout->getGlobalParamsVarLayout();
        slang::TypeLayoutReflection* Reflection = Globals->getTypeLayout();

        const u32 SetCount = Reflection->getDescriptorSetCount();
        for (u32 SetIndex = 0; SetIndex < SetCount; ++SetIndex)
        {
            const u32 RangeCount = Reflection->getDescriptorSetDescriptorRangeCount(SetIndex);
            Array<VkDescriptorSetLayoutBinding> DescriptorSetLayoutBindings;
            for (u32 BindingIndex = 0; BindingIndex < RangeCount; ++BindingIndex)
            {
                const slang::BindingType BindingType = Reflection->getDescriptorSetDescriptorRangeType(SetIndex, BindingIndex);
                if (BindingType == slang::BindingType::PushConstant)
                    continue;

                VkDescriptorSetLayoutBinding Binding { };
                Binding.binding = BindingIndex;
                Binding.descriptorCount = 1;
                Binding.descriptorType = SlangConvertTypeReflectionKind(BindingType);
                Binding.stageFlags = GetShaderStages();


                DescriptorSetLayoutBindings.Add(Binding);
            }

            VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
            DescriptorSetLayoutCreateInfo.bindingCount = DescriptorSetLayoutBindings.Count();
            DescriptorSetLayoutCreateInfo.pBindings = DescriptorSetLayoutBindings.Data();

            VkDescriptorSetLayout SetLayout = nullptr;
            if (VK_FAILED(vkCreateDescriptorSetLayout(Device, &DescriptorSetLayoutCreateInfo, nullptr, &SetLayout)))
            {
                NOVA_VULKAN_ERROR("Failed to create descriptor set layout!");
                return false;
            }
            m_DescriptorSetLayouts.Add(SetLayout);
        }

        if (m_DescriptorSetLayouts.Count() > 0)
        {
            m_DescriptorSets = Array<VkDescriptorSet>(m_DescriptorSetLayouts.Count());

            VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };

            DescriptorSetAllocateInfo.descriptorPool = Renderer->GetDescriptorPool();
            DescriptorSetAllocateInfo.descriptorSetCount = m_DescriptorSetLayouts.Count();
            DescriptorSetAllocateInfo.pSetLayouts = m_DescriptorSetLayouts.Data();
            if (VK_FAILED(vkAllocateDescriptorSets(Device, &DescriptorSetAllocateInfo, m_DescriptorSets.Data())))
            {
                NOVA_VULKAN_ERROR("Failed to allocate descriptor sets!");
                return false;
            }
        }

        VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        PipelineLayoutCreateInfo.pSetLayouts = m_DescriptorSetLayouts.Data();
        PipelineLayoutCreateInfo.setLayoutCount = m_DescriptorSetLayouts.Count();
        PipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
        PipelineLayoutCreateInfo.pushConstantRangeCount = 0;

        if (VK_FAILED(vkCreatePipelineLayout(Device, &PipelineLayoutCreateInfo, nullptr, &m_PipelineLayout)))
        {
            NOVA_VULKAN_ERROR("Failed to create pipeline layout!");
            return false;
        }
        return true;
    }

    bool VulkanShader::Bind()
    {
        const VulkanRenderer* Renderer = dynamic_cast<VulkanRenderer*>(m_Renderer);
        const VkCommandBuffer Cmd = Renderer->GetCurrentCommandBuffer()->GetHandle();
        const VkFunctionPointers& Functions = Renderer->GetFunctionPointers();

        (void)Cmd;
        (void)Functions;
        return true;
    }

    const Array<VulkanShaderModule>& VulkanShader::GetShaderModules() const
    {
        return m_ShaderModules;
    }

    VkPipelineLayout VulkanShader::GetPipelineLayout() const
    {
        return m_PipelineLayout;
    }

    void VulkanShader::Delete()
    {
    }

    i32 VulkanShader::GetUniformLocation(const String& Name)
    {
        return 0;
    }

    String VulkanShader::GetAssetType() const
    {
        return Shader::GetAssetType();
    }

    static SlangStage ConvertShaderStage(ShaderStageFlagBits Stage)
    {
        switch (Stage) {
        case ShaderStageFlagBits::None: return SLANG_STAGE_FRAGMENT;
        case ShaderStageFlagBits::Vertex: return SLANG_STAGE_VERTEX;
        case ShaderStageFlagBits::Geometry: return SLANG_STAGE_GEOMETRY;
        case ShaderStageFlagBits::Fragment: return SLANG_STAGE_FRAGMENT;
        case ShaderStageFlagBits::Compute: return SLANG_STAGE_COMPUTE;
        case ShaderStageFlagBits::RayGeneration: return SLANG_STAGE_RAY_GENERATION;
        case ShaderStageFlagBits::Tessellation: return SLANG_STAGE_HULL;
        case ShaderStageFlagBits::Mesh: return SLANG_STAGE_MESH;
        default: throw;
        }
    }

    bool VulkanShader::FindShaderStage(const StringView& Name, ShaderStageFlagBits Stage)
    {
        slang::IEntryPoint* EntryPoint = nullptr;
        if (SLANG_FAILED(m_ShaderModule->findAndCheckEntryPoint(*Name, ConvertShaderStage(Stage), &EntryPoint, nullptr)))
            return false;

        if (EntryPoint)
        {
            VulkanShaderModule Module;
            Module.Stage = Stage;
            Module.EntryPoint = EntryPoint;
            m_ShaderModules.Add(Module);

            NOVA_LOG(Application, Verbosity::Info, "Shader {}: Found entry point {}.", GetObjectName(), Name);
        }
        return true;
    }

    VkShaderStageFlags VulkanShader::GetShaderStages() const
    {
        const VulkanRenderer* Renderer = m_Renderer->As<VulkanRenderer>();
        VkShaderStageFlags Result = 0;
        for (const VulkanShaderModule& Module : m_ShaderModules)
            Result |= Renderer->Convertor.ConvertShaderStage(Module.Stage);
        return Result;
    }

    const Array<VkDescriptorSet>& VulkanShader::GetDescriptorSets() const
    {
        return m_DescriptorSets;
    }

    const Array<VkDescriptorSetLayout>& VulkanShader::GetDescriptorSetLayouts() const
    {
        return m_DescriptorSetLayouts;
    }

    Array<VkDescriptorSet> VulkanShader::AllocateDescriptorSets() const
    {
        if (m_DescriptorSetLayouts.Count() <= 0)
            return {};

        const VulkanRenderer* Renderer = g_Application->GetRenderer()->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();

        Array<VkDescriptorSet> DescriptorSets = Array<VkDescriptorSet>(m_DescriptorSetLayouts.Count());

        VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};

        DescriptorSetAllocateInfo.descriptorPool = Renderer->GetDescriptorPool();
        DescriptorSetAllocateInfo.descriptorSetCount = m_DescriptorSetLayouts.Count();
        DescriptorSetAllocateInfo.pSetLayouts = m_DescriptorSetLayouts.Data();
        if (VK_FAILED(vkAllocateDescriptorSets(Device, &DescriptorSetAllocateInfo, DescriptorSets.Data())))
        {
            NOVA_VULKAN_ERROR("Failed to allocate descriptor sets!");
            return {};
        }

        return DescriptorSets;
    }

    VkPipelineLayout VulkanShader::CreatePipelineLayout(const Array<VkDescriptorSetLayout>& DescriptorSetLayouts, const Array<VkPushConstantRange>& PushContantRanges) const
    {
        const VulkanRenderer* Renderer = g_Application->GetRenderer()->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();

        VkPipelineLayout PipelineLayout = nullptr;
        VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        PipelineLayoutCreateInfo.pSetLayouts = m_DescriptorSetLayouts.Data();
        PipelineLayoutCreateInfo.setLayoutCount = m_DescriptorSetLayouts.Count();
        PipelineLayoutCreateInfo.pPushConstantRanges = PushContantRanges.Data();
        PipelineLayoutCreateInfo.pushConstantRangeCount = PushContantRanges.Count();

        if (VK_FAILED(vkCreatePipelineLayout(Device, &PipelineLayoutCreateInfo, nullptr, &PipelineLayout)))
        {
            NOVA_VULKAN_ERROR("Failed to create pipeline layout!");
            return nullptr;
        }

        return PipelineLayout;
    }
}
