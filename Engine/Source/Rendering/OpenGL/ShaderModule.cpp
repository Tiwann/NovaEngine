#include "ShaderModule.h"
#include "Conversions.h"
#include "Rendering/RenderDevice.h"
#include <glad/glad.h>


namespace Nova::OpenGL
{
    bool ShaderModule::Initialize(const ShaderModuleCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        if (!createInfo.code) return false;
        if (createInfo.codeSize <= 0) return false;

        if (HandleIsValid(m_Handle))
            glDeleteShader(m_Handle);

        m_Handle = glCreateShader(Convert<GLenum>(createInfo.stage));
        if (m_Handle == GL_INVALID_ENUM) return false;
        glShaderBinary(1, &m_Handle, GL_SHADER_BINARY_FORMAT_SPIR_V, createInfo.code, createInfo.codeSize);
        glSpecializeShader(m_Handle, "main", 0, nullptr, nullptr);
        return true;
    }

    void ShaderModule::Destroy()
    {
    }

    uint32_t ShaderModule::GetHandle() const
    {
        return m_Handle;
    }
}
