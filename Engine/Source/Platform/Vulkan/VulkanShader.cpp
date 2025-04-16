#include "VulkanShader.h"
#include "VulkanRenderer.h"
#include "Runtime/Application.h"
#include "Rendering/ShaderCompiler.h"


namespace Nova
{
    VulkanShader::VulkanShader(Renderer* Renderer, const String& Name, const Path& Filepath) : Shader(Renderer, Name, Filepath)
    {
        slang::IGlobalSession* Slang = m_Renderer->GetOwner()->GetSlangSession();
        slang::TargetDesc TargetDesc = {};
        TargetDesc.format = SLANG_SPIRV;
        TargetDesc.floatingPointMode = SLANG_FLOATING_POINT_MODE_DEFAULT;

        slang::SessionDesc SessionDesc = {};
        SessionDesc.targetCount = 1;
        SessionDesc.targets = &TargetDesc;
        if (SLANG_FAILED(Slang->createSession(SessionDesc, &m_Compiler)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to create Slang session!");
        }
    }

    VulkanShader::~VulkanShader()
    {
        const VulkanRenderer* Renderer = dynamic_cast<VulkanRenderer*>(m_Renderer);
        const VkFunctionPointers& Functions = Renderer->GetFunctionPointers();
        Functions.vkDestroyShaderEXT(Renderer->GetDevice(), m_VertexHandle, nullptr);
        Functions.vkDestroyShaderEXT(Renderer->GetDevice(), m_FragmentHandle, nullptr);
        vkDestroyDescriptorSetLayout(Renderer->GetDevice(), m_DescriptorSetLayout, nullptr);
        vkFreeDescriptorSets(Renderer->GetDevice(), Renderer->GetDescriptorPool(), 1, &m_DescriptorSet);
    }

    bool VulkanShader::Compile()
    {
        if (!m_Compiler)
        {
            return false;
        }
        
        m_ShaderModule = m_Compiler->loadModuleFromSource(*m_Name, m_Filepath.string().c_str(), nullptr, nullptr);
        if (!m_ShaderModule)
        {
            NOVA_LOG(Shader, Verbosity::Error, "Failed to compile shader: Failed to load module");
        }
        
        // At the moment we assume that vertex shaders should have the attribute [shader("vertex")] and entry point should be named vsmain
        // At the moment we assume that fragment shaders should have the attribute [shader("fragment")] and entry point should be named fsmain
        if (SLANG_FAILED(m_ShaderModule->findAndCheckEntryPoint("VertexMain", SLANG_STAGE_VERTEX, &m_VertexEntryPoint, nullptr)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to check vertex entry point!");
            return false;
        }
        
        if (SLANG_FAILED(m_ShaderModule->findAndCheckEntryPoint("FragmentMain", SLANG_STAGE_FRAGMENT, &m_FragmentEntryPoint, nullptr)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to check fragment entry point!");
            return false;
        }
        return true;
    }

    bool VulkanShader::Link()
    {
        if (!m_Compiler) return false;
        
        slang::IComponentType* const ShaderStages[] { m_VertexEntryPoint, m_FragmentEntryPoint };
        if (SLANG_FAILED(m_Compiler->createCompositeComponentType(ShaderStages, std::size(ShaderStages), &m_Program, nullptr)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to create program!");
            return false;
        }
        
        if (SLANG_FAILED(m_Program->link(&m_LinkedProgram, nullptr)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to link program!");
            return false;
        }
        return true;
    }


    bool VulkanShader::Validate()
    {
        if (!m_LinkedProgram) return false;

        const VulkanRenderer* Renderer = dynamic_cast<VulkanRenderer*>(m_Renderer);
        const VkDevice Device = Renderer->GetDevice();
        
        slang::IBlob* CompiledVertexCode = nullptr;
        slang::IBlob* CompiledFragmentCode = nullptr;
        m_LinkedProgram->getEntryPointCode(0, 0, &CompiledVertexCode);
        m_LinkedProgram->getEntryPointCode(1, 0, &CompiledFragmentCode);

        slang::ProgramLayout* Layout = m_LinkedProgram->getLayout();
        if (!Layout)
        {
            NOVA_LOG(Shader, Verbosity::Error, "Failed to get shader layout from Slang.");
            return false;
        }

        const u32 ParameterCount = Layout->getParameterCount();
        Array<VkDescriptorSetLayoutBinding> DescriptorSetLayoutBindings((Array<VkDescriptorSetLayoutBinding>::SizeType)ParameterCount);
        for (u32 i = 0; i < ParameterCount; ++i)
        {
            slang::VariableLayoutReflection* Variable = Layout->getParameterByIndex(i);
            if (!Variable) continue;

            NOVA_LOG(Shader, Verbosity::Trace, "Slang Shader Parameter: {}", Variable->getName());
            const SlangStage Stage = Variable->getStage();
            VkShaderStageFlags VulkanStage = 0;
            if (Stage & SLANG_STAGE_VERTEX) VulkanStage |= VK_SHADER_STAGE_VERTEX_BIT;
            if (Stage & SLANG_STAGE_FRAGMENT) VulkanStage |= VK_SHADER_STAGE_FRAGMENT_BIT;
            if (Stage & SLANG_STAGE_COMPUTE) VulkanStage |= VK_SHADER_STAGE_COMPUTE_BIT;

            DescriptorSetLayoutBindings[i].binding = Variable->getBindingIndex();
            DescriptorSetLayoutBindings[i].stageFlags = VulkanStage;
            DescriptorSetLayoutBindings[i].descriptorCount = 1;

            slang::TypeReflection* Type = Variable->getType();
            if (!Type) continue;

            slang::TypeReflection::Kind Kind = Type->getKind();
            switch (Kind)
            {
            case slang::TypeReflection::Kind::ConstantBuffer:
                DescriptorSetLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                break;
            case slang::TypeReflection::Kind::SamplerState:
                DescriptorSetLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                break;
            case slang::TypeReflection::Kind::Resource:
            case slang::TypeReflection::Kind::TextureBuffer:
                DescriptorSetLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                break;
            case slang::TypeReflection::Kind::ShaderStorageBuffer:
                DescriptorSetLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                break;
            default:
                continue;
            }
        }

        VkDescriptorSetLayoutCreateInfo LayoutCreateInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        LayoutCreateInfo.pBindings = DescriptorSetLayoutBindings.Data();
        LayoutCreateInfo.bindingCount = DescriptorSetLayoutBindings.Count();

        if (VK_FAILED(vkCreateDescriptorSetLayout(Device, &LayoutCreateInfo, nullptr, &m_DescriptorSetLayout)))
        {
            NOVA_LOG(Shader, Verbosity::Error, "Failed to create descriptor set layout!");
            return false;
        }

        const VkFunctionPointers& FunctionPtrs = Renderer->GetFunctionPointers();

        VkShaderCreateInfoEXT VertexCreateInfo= { VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT };
        VertexCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        VertexCreateInfo.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
        VertexCreateInfo.pName = "main";
        VertexCreateInfo.pCode = CompiledVertexCode->getBufferPointer();
        VertexCreateInfo.codeSize = CompiledVertexCode->getBufferSize();
        VertexCreateInfo.pSetLayouts = &m_DescriptorSetLayout;
        VertexCreateInfo.setLayoutCount = 1;
        VertexCreateInfo.pPushConstantRanges = nullptr;
        VertexCreateInfo.pushConstantRangeCount = 0;

        VkShaderCreateInfoEXT FragmentCreateInfo = { VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT };
        FragmentCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        FragmentCreateInfo.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
        FragmentCreateInfo.pName = "main";
        FragmentCreateInfo.pCode = CompiledFragmentCode->getBufferPointer();
        FragmentCreateInfo.codeSize = CompiledFragmentCode->getBufferSize();
        FragmentCreateInfo.pSetLayouts = &m_DescriptorSetLayout;
        FragmentCreateInfo.setLayoutCount = 1;
        FragmentCreateInfo.pPushConstantRanges = nullptr;
        FragmentCreateInfo.pushConstantRangeCount = 0;

        const VkShaderCreateInfoEXT CreateInfos[2] = { VertexCreateInfo, FragmentCreateInfo };
        VkShaderEXT Handles[2] = {};
        if (VK_FAILED(FunctionPtrs.vkCreateShadersEXT(Device, 2, CreateInfos, nullptr, Handles)))
        {
            NOVA_VULKAN_ERROR("Failed to create Vulkan shaders with EXT_shader_object.");
            return false;
        }

        m_VertexHandle = Handles[0];
        m_FragmentHandle = Handles[1];

        VkDescriptorSetAllocateInfo AllocateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        AllocateInfo.descriptorPool = Renderer->GetDescriptorPool();
        AllocateInfo.pSetLayouts = &m_DescriptorSetLayout;
        AllocateInfo.descriptorSetCount = 1;
        if (VK_FAILED(vkAllocateDescriptorSets(Renderer->GetDevice(), &AllocateInfo, &m_DescriptorSet)))
        {
            NOVA_VULKAN_ERROR("Failed to allocate descriptor sets!");
            return false;
        }

        return true;
    }

    bool VulkanShader::Bind()
    {
        const VulkanRenderer* Renderer =dynamic_cast<VulkanRenderer*>(m_Renderer);
        const VkCommandBuffer Cmd = Renderer->GetCurrentCommandBuffer();
        const VkFunctionPointers& Functions = Renderer->GetFunctionPointers();

        constexpr VkShaderStageFlagBits ShaderStages[2] = { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT };
        const VkShaderEXT Shaders[2] = { m_VertexHandle, m_FragmentHandle };
        Functions.vkCmdBindShadersEXT(Cmd, (u32)std::size(ShaderStages), ShaderStages, Shaders);
        return true;
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

    bool VulkanShader::Reload()
    {
        return Shader::Reload();
    }

    void VulkanShader::SetDirectionalLight(const String& Name, const DirectionalLight* DirLight)
    {
        Shader::SetDirectionalLight(Name, DirLight);
    }

    void VulkanShader::SetPointLight(const String& Name, const PointLight* PointLight)
    {
        Shader::SetPointLight(Name, PointLight);
    }

    void VulkanShader::SetAmbientLight(const String& Name, const AmbientLight* AmbientLight)
    {
        Shader::SetAmbientLight(Name, AmbientLight);
    }

    void VulkanShader::SetUniformFloat(const String& Name, f32 Value)
    {
        Shader::SetUniformFloat(Name, Value);
    }

    void VulkanShader::SetUniformFloat2(const String& Name, const Vector2& Value)
    {
        Shader::SetUniformFloat2(Name, Value);
    }

    void VulkanShader::SetUniformFloat3(const String& Name, const Vector3& Value)
    {
        Shader::SetUniformFloat3(Name, Value);
    }

    void VulkanShader::SetUniformFloat4(const String& Name, const Vector4& Value)
    {
        Shader::SetUniformFloat4(Name, Value);
    }

    void VulkanShader::SetUniformMat4(const String& Name, const Matrix4& Value)
    {
        Shader::SetUniformMat4(Name, Value);
    }

    void VulkanShader::SetUniformInt(const String& Name, i32 Value)
    {
        Shader::SetUniformInt(Name, Value);
    }

    void VulkanShader::SetUniformTexture(const String& Name, Texture2D* Texture)
    {
        Shader::SetUniformTexture(Name, Texture);
    }

    void VulkanShader::SetUniformMat2(const String& Name, const Matrix2& Value)
    {
        Shader::SetUniformMat2(Name, Value);
    }

    void VulkanShader::SetUniformMat3(const String& Name, const Matrix3& Value)
    {
        Shader::SetUniformMat3(Name, Value);
    }

    f32 VulkanShader::GetUniformFloat(const String& Name)
    {
        return Shader::GetUniformFloat(Name);
    }

    Vector2 VulkanShader::GetUniformFloat2(const String& Name)
    {
        return Shader::GetUniformFloat2(Name);
    }

    Vector3 VulkanShader::GetUniformFloat3(const String& Name)
    {
        return Shader::GetUniformFloat3(Name);
    }

    Vector4 VulkanShader::GetUniformFloat4(const String& Name)
    {
        return Shader::GetUniformFloat4(Name);
    }

    Matrix4 VulkanShader::GetUniformMat4(const String& Name)
    {
        return Shader::GetUniformMat4(Name);
    }

    i32 VulkanShader::GetUniformInt(const String& Name)
    {
        return Shader::GetUniformInt(Name);
    }

    VkDescriptorSet VulkanShader::GetDescriptorSet() const
    {
        return m_DescriptorSet;
    }
}
