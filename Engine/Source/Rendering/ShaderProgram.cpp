#include "ShaderProgram.h"

namespace Nova::Rendering
{
    void ShaderProgram::AddShaderModule(Rendering::ShaderModule&& shaderModule)
    {
        m_Modules.Emplace(std::move(shaderModule));
    }

    void ShaderProgram::Destroy()
    {
        for (auto& shaderModule : m_Modules)
            shaderModule.Destroy();
    }
}
