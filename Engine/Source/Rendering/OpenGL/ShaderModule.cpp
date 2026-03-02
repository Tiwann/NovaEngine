#include "ShaderModule.h"
#include "Conversions.h"
#include "Rendering/RenderDevice.h"
#include "Runtime/Log.h"

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

        const GLenum shaderStage = Convert<GLenum>(createInfo.stage);
        m_Handle = glCreateShader(shaderStage);
        if (m_Handle == GL_INVALID_ENUM) return false;
#ifdef NOVA_OPENGL_USES_SPIRV
        glShaderBinary(1, &m_Handle, GL_SHADER_BINARY_FORMAT_SPIR_V, createInfo.code, createInfo.codeSize);
        glSpecializeShader(m_Handle, "main", 0, nullptr, nullptr);
#else
        const GLchar* const* code = reinterpret_cast<const GLchar* const*>(&createInfo.code);
        const GLsizei* codeSize = reinterpret_cast<const GLsizei*>(&createInfo.codeSize);
        glShaderSource(m_Handle, 1, code, codeSize);

        glCompileShader(m_Handle);
        GLint sucess = 0;
        glGetShaderiv(m_Handle, GL_COMPILE_STATUS, &sucess);
        if(!sucess)
        {
            GLint length = 0;
            glGetShaderiv(m_Handle, GL_INFO_LOG_LENGTH, &length);
            String log(static_cast<size_t>(length));
            glGetShaderInfoLog(m_Handle, length - 1, &length, log.Data());
            NOVA_LOG(RenderDevice, Verbosity::Error, "Failed to compile shader: {}", log);
            return false;
        }
#endif
        return true;
    }

    void ShaderModule::Destroy()
    {
        glDeleteShader(m_Handle);
        m_Handle = INVALID_HANDLE<uint32_t>;
    }

    uint32_t ShaderModule::GetHandle() const
    {
        return m_Handle;
    }
}
