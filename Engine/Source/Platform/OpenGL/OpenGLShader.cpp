#include "OpenGLShader.h"
#include "Runtime/Assertion.h"
#include "Runtime/Log.h"
#include "Runtime/LogVerbosity.h"
#include "Runtime/Entity.h"
#include "Rendering/Texture2D.h"
#include "Components/Transform.h"
#include "Components/Rendering/DirectionalLight.h"
#include "Containers/StringFormat.h"
#include <glad/gl.h>


namespace Nova
{
    OpenGLShader::OpenGLShader(const String& Name, Path Filepath)
        : Shader(Name, std::move(Filepath)), m_Program(UINT32_MAX), m_VertexHandle(UINT32_MAX), m_FragmentHandle(UINT32_MAX)
    {
        
    }
    
    OpenGLShader::~OpenGLShader()
    {
        glDeleteShader(m_VertexHandle);
        glDeleteShader(m_FragmentHandle);
        glDeleteProgram(m_Program);
    }

    bool OpenGLShader::CompileOpenGL()
    {
        // Compile Vertex
        {
            const String& ShaderName = GetName();
            NOVA_LOG(Shader, Verbosity::Trace, "Compiling vertex shader: {}", ShaderName);
            m_VertexHandle = glCreateShader(GL_VERTEX_SHADER);

            const char* VertexSource = *m_Source.Vertex;
            const i32 VertexSize = (i32)m_Source.Vertex.Count();
            glShaderSource(m_VertexHandle, 1, &VertexSource, &VertexSize);
            glCompileShader(m_VertexHandle);
            
            int Success = 0;
            glGetShaderiv(m_VertexHandle, GL_COMPILE_STATUS, &Success);
            if(!Success)
            {
                i32 Length = 0;
                char Message[GL_INFO_LOG_LENGTH];
                glGetShaderInfoLog(m_VertexHandle, GL_INFO_LOG_LENGTH, &Length, Message);
                NOVA_LOG(Shader, Verbosity::Error, "Failed to compile vertex shader: {}", Message);
                return false;
            }
            
            NOVA_LOG(Shader, Verbosity::Info, "Successfully compiled vertex shader!");
        }

        // Compile Fragment
        {
            const String& ShaderName = GetName();
            NOVA_LOG(Shader, Verbosity::Trace, "Compiling fragment shader: {}", ShaderName);
            m_FragmentHandle = glCreateShader(GL_FRAGMENT_SHADER);

            const char* FragmentSource = *m_Source.Fragment;
            const i32 FragmentSize = (i32)m_Source.Fragment.Count();
            glShaderSource(m_FragmentHandle, 1, &FragmentSource, &FragmentSize);
            glCompileShader(m_FragmentHandle);
            
            int Success = 0;
            glGetShaderiv(m_FragmentHandle, GL_COMPILE_STATUS, &Success);
            if(!Success)
            {
                i32 Length = 0;
                char Message[GL_INFO_LOG_LENGTH];
                glGetShaderInfoLog(m_FragmentHandle, GL_INFO_LOG_LENGTH, &Length, Message);
                NOVA_LOG(Shader, Verbosity::Error, "Failed to compiled fragment shader: {}", Message);
                return false;
            }

            NOVA_LOG(Shader, Verbosity::Info, "Successfully compiled fragment shader!");
        }
        return true;
    }

#define NOVA_USE_DEFAULT_OPENGL_SHADER_COMPILER
    bool OpenGLShader::Compile()
    {
        Compiled =
#if defined(NOVA_USE_DEFAULT_OPENGL_SHADER_COMPILER)
        CompileOpenGL();
#else
        CompileShaderc();
#endif
        m_Program = glCreateProgram();
        glAttachShader(m_Program, m_VertexHandle);
        glAttachShader(m_Program, m_FragmentHandle);
        return Compiled;
    }

    bool OpenGLShader::Link()
    {
        if(!Compiled)
        {
            NOVA_LOG(Shader, Verbosity::Error, "Shader program failed to link: One or more shader(s) failed to compile!");
            return Linked = false;
        }
        
        glLinkProgram(m_Program);
        i32 Success = 0;
        glGetProgramiv(m_Program, GL_LINK_STATUS, &Success);
        if(!Success)
        {
            i32 Length = 0;
            char Message[GL_INFO_LOG_LENGTH];
            glGetProgramInfoLog(m_Program, GL_INFO_LOG_LENGTH, &Length, Message);
            NOVA_LOG(Shader, Verbosity::Error, "Shader program failed to link: {}", Message);
            return Linked = Success;
        }
        const String& ShaderName = GetName();
        NOVA_LOG(Shader, Verbosity::Info, "Shader program {} successfully linked!", ShaderName);
        return Linked = Success;
    }

    bool OpenGLShader::Validate()
    {
        if(!Linked)
        {
            NOVA_LOG(Shader, Verbosity::Error, "Shader program failed to validate: Shader program failed to link!");
            return Validated = false;
        }
        glValidateProgram(m_Program);
        i32 Success = 0;
        glGetProgramiv(m_Program, GL_VALIDATE_STATUS, &Success);
        if(!Success)
        {
            i32 Length = 0;
            char Message[GL_INFO_LOG_LENGTH];
            glGetProgramInfoLog(m_Program, GL_INFO_LOG_LENGTH, &Length, Message);
            NOVA_LOG(Shader, Verbosity::Error, "Shader program failed to validate: {}", Message);
            return Validated = Success;
        }
        NOVA_LOG(Shader, Verbosity::Info, "Shader program {} successfully validated!\n", GetName());
        return Validated = Success;
    }

    bool OpenGLShader::Bind()
    {
        if(!Validated)
        {
            NOVA_LOG(Shader, Verbosity::Error, "Cannot use this shader: Not a valid shader!");
            return false;
        }

        glUseProgram(m_Program);
        return true;
    }

    void OpenGLShader::Delete()
    {
        glDeleteShader(m_VertexHandle);
        glDeleteShader(m_FragmentHandle);
        glDeleteProgram(m_Program);
    }

    void OpenGLShader::SetDirectionalLight(const String& Name, const DirectionalLight* DirLight)
    {
        if (!DirLight) return;
        const Transform* Transform = DirLight->GetTransform();
        const bool Enabled = DirLight->GetOwner()->IsEnabled() && DirLight->IsEnabled();
        const f32 Intensity = Enabled ? DirLight->GetIntensity() : 0.0f;
        const Color Color = Enabled ? DirLight->GetColor() : Color::Black;
        
        SetUniformFloat3(StringFormat("{}.direction", Name), Transform->GetForwardVector());
        SetUniformFloat(StringFormat("{}.intensity", Name), Intensity);
        SetUniformFloat4(StringFormat("{}.color", Name), Color);
    }

    void OpenGLShader::SetPointLight(const String& Name, const PointLight* PointLight)
    {
        if (!PointLight) return;
        const bool Enabled = PointLight->GetOwner()->IsEnabled() && PointLight->IsEnabled();
        const f32 Intensity = Enabled ? PointLight->GetIntensity() : 0.0f;
        const Color Color = Enabled ? PointLight->GetColor() : Color::Black;

        SetUniformFloat(StringFormat("{}.intensity", Name), Intensity);
        SetUniformFloat4(StringFormat("{}.color", Name), Color);
        SetUniformFloat(StringFormat("{}.innerRadius", Name), PointLight->GetInnerRadius());
        SetUniformFloat(StringFormat("{}.outerRadius", Name), PointLight->GetOuterRadius());
    }

    void OpenGLShader::SetAmbientLight(const String& Name, const AmbientLight* AmbientLight)
    {
        if (!AmbientLight) return;
        const bool Enabled = AmbientLight->GetOwner()->IsEnabled() && AmbientLight->IsEnabled();
        const f32 Intensity = Enabled ? AmbientLight->GetIntensity() : 0.0f;
        const Color Color = Enabled ? AmbientLight->GetColor() : Color::Black;

        SetUniformFloat(StringFormat("{}.intensity", Name), Intensity);
        SetUniformFloat4(StringFormat("{}.color", Name), Color);
    }

#define NOVA_SHADER_UNIFORM_CHECK(Uniform, Name) \
        if((Uniform) == -1) \
        { \
            NOVA_LOG(Shader, Verbosity::Error, "Uniform \"{}\" doesn't exist in shader \"{}\"", (Name), m_Name); \
            return; \
        }((void)0)
    
    
    void OpenGLShader::SetUniformFloat(const String& Name, f32 Value)
    {
        const i32 Location = glGetUniformLocation(m_Program, *Name);
        NOVA_SHADER_UNIFORM_CHECK(Location, Name);
        glUniform1f(Location, Value);
    }
    

    void OpenGLShader::SetUniformFloat2(const String& Name, const Vector2& Value)
    {
        const i32 Location = glGetUniformLocation(m_Program, *Name);
        NOVA_SHADER_UNIFORM_CHECK(Location, Name);
        glUniform2f(Location, Value.x, Value.y);
    }

    void OpenGLShader::SetUniformFloat3(const String& Name, const Vector3& Value)
    {
        const i32 Location = glGetUniformLocation(m_Program, *Name);
        NOVA_SHADER_UNIFORM_CHECK(Location, Name);
        glUniform3f(Location, Value.x, Value.y, Value.z);
    }

    void OpenGLShader::SetUniformFloat4(const String& Name, const Vector4& Value)
    {
        const i32 Location = glGetUniformLocation(m_Program, *Name);
        NOVA_SHADER_UNIFORM_CHECK(Location, Name);
        glUniform4fv(Location, 1, (const f32*)&Value);
    }

    void OpenGLShader::SetUniformMat4(const String& Name, const Matrix4& Value)
    {
        const i32 Location = glGetUniformLocation(m_Program, *Name);
        NOVA_SHADER_UNIFORM_CHECK(Location, Name);
        glUniformMatrix4fv(Location, 1, false, (const f32*)&Value);
    }

    void OpenGLShader::SetUniformInt(const String& Name, i32 Value)
    {
        const i32 Location = glGetUniformLocation(m_Program, *Name);
        NOVA_SHADER_UNIFORM_CHECK(Location, Name);
        glUniform1i(Location, Value);
    }

    void OpenGLShader::SetUniformTexture(const String& Name, Texture2D* Texture)
    {
        if(!Texture)
        {
            NOVA_LOG(Shader, Verbosity::Error, "Tried to upload a nullptr texture to a shader!");
            return;
        }
        Texture->Bind();
        SetUniformInt(Name, (i32)Texture->GetSlot());
    }

    void OpenGLShader::SetUniformMat2(const String& Name, const Matrix2& Value)
    {
        const i32 Location = glGetUniformLocation(m_Program, *Name);
        NOVA_SHADER_UNIFORM_CHECK(Location, Name);
        glUniformMatrix2fv(Location, 1, false, (const f32*)&Value);
    }

    void OpenGLShader::SetUniformMat3(const String& Name, const Matrix3& Value)
    {
        const i32 Location = glGetUniformLocation(m_Program, *Name);
        NOVA_SHADER_UNIFORM_CHECK(Location, Name);
        glUniformMatrix3fv(Location, 1, false, (const f32*)&Value);
    }

    f32 OpenGLShader::GetUniformFloat(const String& Name)
    {
        const i32 Location = glGetUniformLocation(m_Program, *Name);
        if(!Location) \
        { \
            NOVA_LOG(Shader, Verbosity::Error, "Uniform \"{}\" doesn't exist in shader \"{}\"", (Name), m_Name); \
            NOVA_LOG(Shader, Verbosity::Error, "Path: {}", m_Filepath.string()); \
            return 0.0f; \
        }

        f32 Result;
        glGetUniformfv(m_Program, Location, &Result);
        return Result;
    }

    Vector2 OpenGLShader::GetUniformFloat2(const String& Name)
    {
        const i32 Location = glGetUniformLocation(m_Program, *Name);
        if(!Location) \
        { \
            NOVA_LOG(Shader, Verbosity::Error, "Uniform \"{}\" doesn't exist in shader \"{}\"", (Name), m_Name); \
            NOVA_LOG(Shader, Verbosity::Error, "Path: {}", m_Filepath.string()); \
            return {0.0f}; \
        }

        Vector2 Result;
        glGetnUniformfv(m_Program, Location, 2 * sizeof(f32), (f32*)&Result);
        return Result;
    }

    Vector3 OpenGLShader::GetUniformFloat3(const String& Name)
    {
        const i32 Location = glGetUniformLocation(m_Program, *Name);
        if(!Location) \
        { \
            NOVA_LOG(Shader, Verbosity::Error, "Uniform \"{}\" doesn't exist in shader \"{}\"", (Name), m_Name); \
            NOVA_LOG(Shader, Verbosity::Error, "Path: {}", m_Filepath.string()); \
            return {0.0f}; \
        }

        Vector3 Result;
        glGetnUniformfv(m_Program, Location, 3 * sizeof(f32), (f32*)&Result);
        return Result;
    }

    Vector4 OpenGLShader::GetUniformFloat4(const String& Name)
    {
        const i32 Location = glGetUniformLocation(m_Program, *Name);
        if(!Location) \
        { \
            NOVA_LOG(Shader, Verbosity::Error, "Uniform \"{}\" doesn't exist in shader \"{}\"", (Name), m_Name); \
            NOVA_LOG(Shader, Verbosity::Error, "Path: {}", m_Filepath.string()); \
            return {0.0f}; \
        }

        Vector4 Result;
        glGetnUniformfv(m_Program, Location, 4 * sizeof(f32), (f32*)&Result);
        return Result;
    }

    Matrix4 OpenGLShader::GetUniformMat4(const String& Name)
    {
        const i32 Location = glGetUniformLocation(m_Program, *Name);
        if(!Location) \
        { \
            NOVA_LOG(Shader, Verbosity::Error, "Uniform \"{}\" doesn't exist in shader \"{}\"", (Name), m_Name); \
            NOVA_LOG(Shader, Verbosity::Error, "Path: {}", m_Filepath.string()); \
            return Matrix4::Identity; \
        }

        Matrix4 Result;
        glGetnUniformfv(m_Program, Location, 4 * 4 * sizeof(f32), (f32*)&Result);
        return Result;
    }

    i32 OpenGLShader::GetUniformInt(const String& Name)
    {
        const i32 Location = glGetUniformLocation(m_Program, *Name);
        if(!Location) \
        { \
            NOVA_LOG(Shader, Verbosity::Error, "Uniform \"{}\" doesn't exist in shader \"{}\"", Name, m_Name); \
            NOVA_LOG(Shader, Verbosity::Error, "Path: {}", m_Filepath.string()); \
            return 0; \
        }

        i32 Result;
        glGetUniformiv(m_Program, Location, &Result);
        return Result;
    }

    i32 OpenGLShader::GetUniformLocation(const String& Name)
    {
        return glGetUniformLocation(m_Program, *Name);
    }
}
