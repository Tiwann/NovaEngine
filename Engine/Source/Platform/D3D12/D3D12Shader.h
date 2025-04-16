#pragma once
#include "Rendering/Shader.h"

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
        i32 GetUniformLocation(const String& Name) override;
    };
}
