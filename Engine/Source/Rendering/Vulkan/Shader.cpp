#include "Containers/StringFormat.h"
#include "Shader.h"
#include "Buffer.h"
#include "DescriptorPool.h"
#include "Device.h"
#include "Sampler.h"

#include <vulkan/vulkan.h>

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
        m_EntryPoints.Clear();
        m_ShaderModules.Clear();
        m_BindingSetLayouts.Clear();
        m_BindingSets.Clear();

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
        }


        slang::ProgramLayout* programLayout = m_LinkedProgram->getLayout();
        slang::VariableLayoutReflection* varLayout = programLayout->getGlobalParamsVarLayout();
        slang::TypeLayoutReflection* typeLayout = varLayout->getTypeLayout();

        ShaderStageFlags stageFlags = ShaderStageFlagBits::None;
        for (int e = 0; e < programLayout->getEntryPointCount(); ++e)
        {
            slang::EntryPointReflection* ep = programLayout->getEntryPointByIndex(e);
            stageFlags |= GetStage(ep->getStage());
        }

        const uint32_t setCount = typeLayout->getDescriptorSetCount();
        for (uint32_t setIndex = 0; setIndex < setCount; ++setIndex)
        {
            const uint32_t bindingCount = typeLayout->getDescriptorSetDescriptorRangeCount(setIndex);


            ShaderBindingSetLayout bindingSetLayout;
            for (uint32_t bindingIndex = 0; bindingIndex < bindingCount; ++bindingIndex)
            {
                const BindingType bindingType = GetBindingType(typeLayout->getDescriptorSetDescriptorRangeType(setIndex, bindingIndex));
                if (bindingType == BindingType::PushConstant)
                {
                    continue;
                }

                size_t bindingIndexOffset = typeLayout->getDescriptorSetDescriptorRangeIndexOffset(setIndex, bindingIndex);

                const uint32_t descriptorCount = typeLayout->getDescriptorSetDescriptorRangeDescriptorCount(setIndex, bindingIndex);
                bindingSetLayout.SetBinding(bindingIndex, { "", stageFlags, bindingType, descriptorCount });
            }

            if (bindingSetLayout.BindingCount() > 0)
            {
                bindingSetLayout.Initialize(createInfo.device);
                bindingSetLayout.Build();
                m_BindingSetLayouts.Add(bindingSetLayout);
            }
        }

        Device* device = (Device*)createInfo.device;
        const DescriptorPool* descriptorPool = device->GetDescriptorPool();
        for (const ShaderBindingSetLayout& bindingSetLayout : m_BindingSetLayouts)
        {
            ShaderBindingSet bindingSet;
            bindingSet.Initialize(*descriptorPool, bindingSetLayout);
            m_BindingSets.Add(bindingSet);
        }

        const auto toDescriptorSetLayouts = [](const ShaderBindingSetLayout& setLayout)
        {
            return setLayout.GetHandle();
        };

        Array<VkDescriptorSetLayout> descriptorSetLayouts = m_BindingSetLayouts.Transform<VkDescriptorSetLayout>(toDescriptorSetLayouts);

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutCreateInfo.setLayoutCount = descriptorSetLayouts.Count();
        pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.Data();
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
        vkCreatePipelineLayout(device->GetHandle(), &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout);

        m_Device = device;
        return true;
    }

    void Shader::Destroy()
    {
        for (auto& bindingSetLayout : m_BindingSetLayouts)
            bindingSetLayout.Destroy();

        for (auto& bindingSet : m_BindingSets)
            bindingSet.Destroy();

        for (auto& shaderModule : m_ShaderModules)
            shaderModule.Destroy();

        vkDestroyPipelineLayout(m_Device->GetHandle(), m_PipelineLayout, nullptr);
    }

    bool Shader::BindTexture(const uint32_t set, const uint32_t binding, const Rendering::Texture& texture)
    {
        if (set > m_BindingSets.Count())
            return false;

        VkDescriptorImageInfo imageInfo;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = ((const Texture&)texture).GetImageView();

        VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        if (texture.GetUsageFlags().Contains(Rendering::TextureUsageFlagBits::Storage))
            descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

        VkWriteDescriptorSet write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write.descriptorType = descriptorType;
        write.descriptorCount = 1;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.pImageInfo = &imageInfo;
        write.dstSet = m_BindingSets[set].GetHandle();
        vkUpdateDescriptorSets(m_Device->GetHandle(), 1, &write, 0, nullptr);
        return true;
    }

    bool Shader::BindSampler(const uint32_t set, const uint32_t binding, const Rendering::Sampler& sampler)
    {
        if (set > m_BindingSets.Count())
            return false;

        VkDescriptorImageInfo imageInfo;
        imageInfo.sampler = ((const Sampler&)sampler).GetHandle();

        VkWriteDescriptorSet write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        write.descriptorCount = 1;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.pImageInfo = &imageInfo;
        write.dstSet = m_BindingSets[set].GetHandle();
        vkUpdateDescriptorSets(m_Device->GetHandle(), 1, &write, 0, nullptr);
        return true;
    }

    bool Shader::BindCombinedSamplerTexture(const uint32_t set, const uint32_t binding, const Rendering::Sampler& sampler, const Rendering::Texture& texture)
    {
        if (set > m_BindingSets.Count())
            return false;

        VkDescriptorImageInfo imageInfo;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = ((const Texture&)texture).GetImageView();
        imageInfo.sampler = ((const Sampler&)sampler).GetHandle();

        VkWriteDescriptorSet write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = 1;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.pImageInfo = &imageInfo;
        write.dstSet = m_BindingSets[set].GetHandle();
        vkUpdateDescriptorSets(m_Device->GetHandle(), 1, &write, 0, nullptr);
        return true;
    }

    bool Shader::BindBuffer(const uint32_t set, const uint32_t binding, const Rendering::Buffer& buffer, const size_t offset, const size_t size)
    {
        if (set > m_BindingSets.Count())
            return false;

        VkDescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = ((const Buffer&)buffer).GetHandle();
        bufferInfo.offset = offset;
        bufferInfo.range = size;

        VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
        switch (buffer.GetUsage())
        {
        case Rendering::BufferUsage::None:
        case Rendering::BufferUsage::VertexBuffer:
        case Rendering::BufferUsage::IndexBuffer:
        case Rendering::BufferUsage::UniformBuffer:
            descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            break;
        case Rendering::BufferUsage::StorageBuffer:
            descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            break;
        case Rendering::BufferUsage::StagingBuffer:
            break;
        }

        if (descriptorType == VK_DESCRIPTOR_TYPE_MAX_ENUM)
            return false;


        VkWriteDescriptorSet write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write.descriptorType = descriptorType;
        write.descriptorCount = 1;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.pBufferInfo = &bufferInfo;
        write.dstSet = m_BindingSets[set].GetHandle();
        vkUpdateDescriptorSets(m_Device->GetHandle(), 1, &write, 0, nullptr);
        return true;
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

    const Array<ShaderBindingSet>& Shader::GetBindingSets() const
    {
        return m_BindingSets;
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
        return m_BindingSetLayouts.Transform<VkDescriptorSetLayout>([](const ShaderBindingSetLayout& bindingSetLayout) { return bindingSetLayout.GetHandle(); });
    }

    Array<VkDescriptorSet> Shader::GetDescriptorSets() const
    {
        return m_BindingSets.Transform<VkDescriptorSet>([](const ShaderBindingSet& bindingSet) { return bindingSet.GetHandle(); });
    }
}
