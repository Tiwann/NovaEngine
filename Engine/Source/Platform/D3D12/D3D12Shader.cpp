#include "D3D12Shader.h"

namespace Nova
{
    D3D12Shader::D3D12Shader(const String& Name, const Path& Filepath) : Shader(Name, Filepath)
    {
    }

    bool D3D12Shader::Compile()
    {
        return true;
    }

    bool D3D12Shader::Link()
    {
        return true;
    }

    bool D3D12Shader::Validate()
    {
        return true;
    }

    bool D3D12Shader::Bind()
    {
        return true;
    }

    void D3D12Shader::Delete()
    {
    }

    i32 D3D12Shader::GetUniformLocation(const String& Name)
    {
        return 0;
    }

}