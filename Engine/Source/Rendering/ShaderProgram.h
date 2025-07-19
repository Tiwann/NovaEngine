#pragma once
#include "ShaderModule.h"
#include "Containers/Array.h"

namespace Nova::Rendering
{
    class ShaderProgram
    {
    public:
        ShaderProgram() = default;

        void AddShaderModule(Rendering::ShaderModule&& shaderModule);
        void Destroy();
    private:
        Array<Rendering::ShaderModule> m_Modules;
    };
}
