#pragma once
#include "Rendering/Shader.h"
#include <vulkan/vulkan.h>

#include "Containers/StringView.h"


struct ISlangBlob;

namespace slang
{
    struct IEntryPoint;
    typedef ISlangBlob IBlob;
    struct ISession;
    struct IModule;
    struct IComponentType;
}

namespace Nova
{
    struct ShaderModule
    {
        ShaderStage Stage = ShaderStage::None;
        i32 EntryPointIndex = 0xFFFFFFFF;
        slang::IEntryPoint* EntryPoint = nullptr;
        slang::IBlob* CompiledCode = nullptr;
        VkShaderEXT Handle = nullptr;
        VkShaderModule Module = nullptr;
    };

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

        void SetDirectionalLight(const String& Name, const DirectionalLight* DirLight) override;
        void SetPointLight(const String& Name, const PointLight* PointLight) override;
        void SetAmbientLight(const String& Name, const AmbientLight* AmbientLight) override;
        void SetUniformFloat(const String& Name, f32 Value) override;
        void SetUniformFloat2(const String& Name, const Vector2& Value) override;
        void SetUniformFloat3(const String& Name, const Vector3& Value) override;
        void SetUniformFloat4(const String& Name, const Vector4& Value) override;
        void SetUniformMat4(const String& Name, const Matrix4& Value) override;
        void SetUniformInt(const String& Name, i32 Value) override;
        void SetUniformTexture(const String& Name, Texture2D* Texture) override;
        void SetUniformMat2(const String& Name, const Matrix2& Value) override;
        void SetUniformMat3(const String& Name, const Matrix3& Value) override;
        f32 GetUniformFloat(const String& Name) override;
        Vector2 GetUniformFloat2(const String& Name) override;
        Vector3 GetUniformFloat3(const String& Name) override;
        Vector4 GetUniformFloat4(const String& Name) override;
        Matrix4 GetUniformMat4(const String& Name) override;
        i32 GetUniformInt(const String& Name) override;

        const Array<ShaderModule>& GetShaderModules() const;
    private:
        bool FindShaderStage(const StringView& Name, ShaderStage Stage);
    private:
        slang::ISession* m_Compiler = nullptr;
        slang::IModule* m_ShaderModule = nullptr;
        slang::IComponentType* m_Program = nullptr;
        slang::IComponentType* m_LinkedProgram = nullptr;
        Array<ShaderModule> m_ShaderModules;
    };
}
