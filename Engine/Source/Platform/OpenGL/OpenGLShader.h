#pragma once
#include "Containers/StringView.h"
#include "Rendering/Shader.h"

struct ISlangBlob;

namespace slang
{
    struct ISession;
    struct IModule;
    struct IEntryPoint;
    struct IComponentType;
    typedef ISlangBlob IBlob;
}

namespace Nova
{

    struct OpenGLShaderModule
    {
        u32 Handle = -1;
        ShaderStage Stage = ShaderStage::None;
        slang::IEntryPoint* EntryPoint = nullptr;
        slang::IBlob* CompiledCode;
    };


    class OpenGLShader : public Shader
    {
    public:
        explicit OpenGLShader(Renderer* Renderer, const String& Name, Path Filepath);
        
        ~OpenGLShader() override;
        bool Compile() override;
        bool Link() override;
        bool Validate() override;
        bool Bind() override;
        void Delete() override;

        bool FindShaderStage(const StringView& Name, ShaderStage Stage);
    protected:
        slang::ISession* m_Session = nullptr;
        slang::IModule* m_Module = nullptr;
        slang::IComponentType* m_Program = nullptr;
        slang::IComponentType* m_LinkedProgram = nullptr;
        Array<OpenGLShaderModule> m_ShaderModules;
        u32 m_ProgramHandle = U32_MAX;
    };
}
