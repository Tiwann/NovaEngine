#pragma once
#include "Containers/StringView.h"
#include "Rendering/Shader.h"
#include "Rendering/ShaderModule.h"


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
    class D3D12Shader : public Shader
    {
    public:
        explicit D3D12Shader(Renderer* Renderer, const String& Name, const Path& Filepath);

        bool Compile() override;
        bool Link() override;
        bool Validate() override;
        bool Bind() override;
        void Delete() override;

        const D3D12ShaderModule* GetShaderModule(ShaderStage Stage) const;
        const D3D12ShaderModule* GetVertexShaderModule() const;
        const D3D12ShaderModule* GetFragmentShaderModule() const;
    private:
        bool FindShaderStage(const StringView& Name, ShaderStage Stage);
    private:
        slang::ISession* m_Compiler = nullptr;
        slang::IModule* m_ShaderModule = nullptr;
        slang::IComponentType* m_Program = nullptr;
        slang::IComponentType* m_LinkedProgram = nullptr;
        Array<D3D12ShaderModule> m_ShaderModules;
    };
}
