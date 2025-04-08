#pragma once
#include "Graphics/Shader.h"

namespace Nova
{
    class DirectXShader : public Shader
    {
    public:
        DirectXShader(const String& Name, const Path& Filepath);

        bool Compile() override;
        bool Link() override;
        bool Validate() override;
        bool Bind() override;
        void Delete() override;
        i32 GetUniformLocation(const String& Name) override;
    };
}
