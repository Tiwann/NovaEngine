#include "D3D12Shader.h"
#include "Runtime/Application.h"
#include "D3D12Renderer.h"
#include "Runtime/Log.h"
#include <slang/slang.h>


namespace Nova
{
    static slang::IBlob* DiagnosticsBlob = nullptr;
    static StringView GetDiagnostic(slang::IBlob* InDiagnostic)
    {
        if (!InDiagnostic)
            return nullptr;

        return StringView((const char*)InDiagnostic->getBufferPointer(), InDiagnostic->getBufferSize());
    }

    D3D12Shader::D3D12Shader(Renderer* Renderer, const String& Name, const Path& Filepath) : Shader(Renderer, Name, Filepath)
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

    bool D3D12Shader::Compile()
    {
        if (!m_Compiler)
            return false;

        m_ShaderModule = m_Compiler->loadModuleFromSource(*m_Name, m_Filepath.string().c_str(), nullptr, &DiagnosticsBlob);
        if (!m_ShaderModule)
        {
            NOVA_LOG(Shader, Verbosity::Error, "Failed to compile shader: Failed to load module");
            NOVA_LOG(Shader, Verbosity::Error, "{}", GetDiagnostic(DiagnosticsBlob));
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

    bool D3D12Shader::Link()
    {
        if (!m_Compiler) return false;

        Array<slang::IComponentType*> EntryPoints;
        for (const D3D12ShaderModule& Module : m_ShaderModules)
            EntryPoints.Add(Module.EntryPoint);

        if (SLANG_FAILED(m_Compiler->createCompositeComponentType(EntryPoints.Data(), EntryPoints.Count(), &m_Program, &DiagnosticsBlob)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to create program!");
            NOVA_LOG(Shader, Verbosity::Error, "{}", GetDiagnostic(DiagnosticsBlob));
            return false;
        }


        if (SLANG_FAILED(m_Program->link(&m_LinkedProgram, &DiagnosticsBlob)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to link program!");
            NOVA_LOG(Shader, Verbosity::Error, "{}", GetDiagnostic(DiagnosticsBlob));
            return false;
        }
        return true;
    }

    bool D3D12Shader::Validate()
    {
        if (!m_LinkedProgram) return false;

        for (size_t i = 0; i < m_ShaderModules.Count(); i++)
        {
            const SlangResult Result = m_LinkedProgram->getEntryPointCode(i, 0, &m_ShaderModules[i].CompiledCode, &DiagnosticsBlob);
            if (SLANG_FAILED(Result))
            {
                NOVA_LOG(Application, Verbosity::Error, "Failed to get entry point code!");
                NOVA_LOG(Shader, Verbosity::Error, "{}", GetDiagnostic(DiagnosticsBlob));
                return false;
            }
        }




        /*slang::ProgramLayout* Layout = m_LinkedProgram->getLayout();
        slang::VariableLayoutReflection* Globals = Layout->getGlobalParamsVarLayout();
        slang::TypeLayoutReflection* Reflection = Globals->getTypeLayout();

        for (u32 SetIndex = 0; SetIndex < Reflection->getDescriptorSetCount(); ++SetIndex)
        {
            const u32 RangeCount = Reflection->getDescriptorSetDescriptorRangeCount(SetIndex);
            Array<VkDescriptorSetLayoutBinding> DescriptorSetLayoutBindings;
            for (u32 BindingIndex = 0; BindingIndex < RangeCount; ++BindingIndex)
            {
                const slang::BindingType BindingType = Reflection->getDescriptorSetDescriptorRangeType(SetIndex, BindingIndex);

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
        }*/
        return true;
    }

    bool D3D12Shader::Bind()
    {
        return true;
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

    bool D3D12Shader::FindShaderStage(const StringView& Name, ShaderStageFlagBits Stage)
    {
        slang::IEntryPoint* EntryPoint = nullptr;
        if (SLANG_FAILED(m_ShaderModule->findAndCheckEntryPoint(*Name, ConvertShaderStage(Stage), &EntryPoint, nullptr)))
            return false;

        if (EntryPoint)
        {
            D3D12ShaderModule Module;
            Module.Stage = Stage;
            Module.EntryPoint = EntryPoint;
            m_ShaderModules.Add(Module);
            NOVA_LOG(Application, Verbosity::Info, "Shader {}: Found entry point {}.", GetObjectName(), Name);
        }
        return true;
    }

    void D3D12Shader::Delete()
    {
    }

    const D3D12ShaderModule* D3D12Shader::GetShaderModule(ShaderStageFlagBits Stage) const
    {
        return m_ShaderModules.Single([&Stage](const D3D12ShaderModule& Module) { return Module.Stage == Stage; });
    }

    const D3D12ShaderModule* D3D12Shader::GetVertexShaderModule() const
    {
        return GetShaderModule(ShaderStageFlagBits::Vertex);
    }

    const D3D12ShaderModule* D3D12Shader::GetFragmentShaderModule() const
    {
        return GetShaderModule(ShaderStageFlagBits::Fragment);
    }
}
