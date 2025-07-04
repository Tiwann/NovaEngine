#pragma once
#include "Rendering/Shader.h"
#include "Rendering/ShaderModule.h"
#include "Containers/StringView.h"
#include <vulkan/vulkan.h>

namespace slang
{
    struct IEntryPoint;
    typedef struct ::ISlangBlob IBlob;
    struct ISession;
    struct IModule;
    struct IComponentType;
}

namespace Nova
{
    class VulkanShader : public Shader
    {
    public:
        VulkanShader(Renderer* Renderer, const String& Name, const Path& Filepath);
        ~VulkanShader() override;

        bool Compile() override;
        bool Link() override;
        bool Validate() override;
        bool Bind() override;
        void Delete() override;

        i32 GetUniformLocation(const String& Name) override;
        String GetAssetType() const override;

        const Array<VulkanShaderModule>& GetShaderModules() const;

        VkPipelineLayout GetPipelineLayout() const;
        VkShaderStageFlags GetShaderStages() const;
        const Array<VkDescriptorSet>& GetDescriptorSets() const;
        const Array<VkDescriptorSetLayout>& GetDescriptorSetLayouts() const;
        Array<VkDescriptorSet> AllocateDescriptorSets() const;
    private:
        bool FindShaderStage(const StringView& Name, ShaderStage Stage);

    private:
        slang::ISession* m_Compiler = nullptr;
        slang::IModule* m_ShaderModule = nullptr;
        slang::IComponentType* m_Program = nullptr;
        slang::IComponentType* m_LinkedProgram = nullptr;
        Array<VulkanShaderModule> m_ShaderModules;
        Array<VkDescriptorSet> m_DescriptorSets;
        Array<VkDescriptorSetLayout> m_DescriptorSetLayouts;
        VkPipelineLayout m_PipelineLayout = nullptr;
    };
}
