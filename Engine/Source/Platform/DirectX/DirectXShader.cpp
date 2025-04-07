#include "DirectXShader.h"

namespace Nova
{
    DirectXShader::DirectXShader(const String& Name, const Path& Filepath) : Shader(Name, Filepath)
    {
    }

    bool DirectXShader::Compile()
    {
        return true;
    }

    bool DirectXShader::Link()
    {
        return true;
    }

    bool DirectXShader::Validate()
    {
        return true;
    }

    bool DirectXShader::Bind()
    {
        return true;
    }

    void DirectXShader::Delete()
    {
    }

    i32 DirectXShader::GetUniformLocation(const String& Name)
    {
        return 0;
    }

}