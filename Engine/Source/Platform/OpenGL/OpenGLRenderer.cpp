#include "OpenGLRenderer.h"
#include "Runtime/Application.h"
#include "Runtime/Window.h"
#include "Runtime/Log.h"
#include "Runtime/Color.h"
#include "Runtime/LogVerbosity.h"
#include "Rendering/Vertex.h"

#include "Components/Camera.h"
#include "Rendering/Shader.h"
#include "Rendering/IndexBuffer.h"
#include "ResourceManager/ShaderManager.h"
#include "Rendering/VertexArray.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/VertexBufferLayout.h"
#include "Math/LinearAlgebra.h"
#include "Containers/ScopedPointer.h"
#include "Containers/StaticArray.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>


namespace Nova
{
    bool OpenGLRenderer::Initialize()
    {
        NOVA_LOG(OpenGL, Verbosity::Trace, "Creating OpenGL context");
        glfwMakeContextCurrent(m_Application->GetWindow()->GetNativeWindow());
        glfwSwapInterval(m_Application->GetConfiguration().Graphics.VSync);
        
        if(!gladLoadGL(glfwGetProcAddress))
        {
            NOVA_LOG(OpenGL, Verbosity::Error, "Failed to retrieve OpenGL function pointers!");
            m_Application->RequireExit(ExitCode::Error);
            return false;
        }


        const auto Callback = [](GLenum Source, GLenum, GLuint, GLenum Severity, GLsizei, const GLchar *Message, const void *) -> void
        {
            const Verbosity Verbo = GetDebugVerbosity(Severity);
            const String SourceName = GetDebugSourceName(Source);
            if(Verbo < Verbosity::Warning)
                return;
            NOVA_LOG(OpenGL, Verbo, "Debug ({}): {}", SourceName, Message);
        };

#if defined(NOVA_DEBUG)
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(Callback, nullptr);
#endif
        
        glEnable(GL_MULTISAMPLE);
        const String Renderer = (const char*)glGetString(GL_RENDERER);
        NOVA_LOG(OpenGL, Verbosity::Info, "Using OpenGL 4.6");
        NOVA_LOG(OpenGL, Verbosity::Info, "Using GPU: {}", Renderer);

        
        return true;
    }

    void OpenGLRenderer::Destroy()
    {
    }

    void OpenGLRenderer::ClearDepth(float Depth)
    {
        glClearDepthf(Depth);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRenderer::ClearColor(const Color& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void OpenGLRenderer::Present()
    {
        glfwSwapBuffers(m_Application->GetWindow()->GetNativeWindow());
    }

    bool OpenGLRenderer::BeginFrame()
    {
        return true;
    }

    void OpenGLRenderer::EndFrame()
    {
    }

    void OpenGLRenderer::SetViewportRect(Vector2 Position, Vector2 Size)
    {
        glViewport((GLint)Position.x, (GLint)Position.y, (GLint)Size.x, (GLint)Size.y);
    }

    void OpenGLRenderer::Draw(DrawMode Mode, VertexArray* VertexArray, u32 NumVert, Shader* Shader)
    {
        VertexArray->Bind();
        Shader->Bind();
        if(m_CurrentCamera)
        {
            Shader->SetUniformMat4("uView", m_CurrentCamera->GetViewMatrix());
            Shader->SetUniformMat4("uProjection", m_CurrentCamera->GetProjectionMatrix());
        } else
        {
            NOVA_LOG(OpenGL, Verbosity::Warning, "No camera component found! No view-projection matrix sent.");
        }

        glDrawArrays(ConvertDrawMode(Mode), 0, (i32)NumVert);
    }

    void OpenGLRenderer::DrawIndexed(DrawMode Mode, VertexArray* VertexArray, VertexBuffer* VertexBuffer, IndexBuffer* IndexBuffer, Shader* Shader)
    {
        VertexArray->Bind();
        VertexBuffer->Bind();
        IndexBuffer->Bind();
        Shader->Bind();
        if(m_CurrentCamera)
        {
            Shader->SetUniformMat4("uView", m_CurrentCamera->GetViewMatrix());
            Shader->SetUniformMat4("uProjection", m_CurrentCamera->GetProjectionMatrix());
        } else
        {
            NOVA_LOG(OpenGL, Verbosity::Warning, "No camera component found! No view-projection matrix sent.");
        }
        glDrawElements(ConvertDrawMode(Mode), (GLsizei)IndexBuffer->Count(), GL_UNSIGNED_INT, nullptr);
    }

    void OpenGLRenderer::DrawLine(const Vector3& PointA, const Vector3& PointB, f32 Thickness, const Color& Color)
    {
        ScopedPointer<VertexArray> VertexArray = VertexArray::Create(m_GraphicsApi);
        VertexArray->Bind();
        const Vertex Points[] {
            Vertex { PointA, Vector2::Zero, Vector3::Zero, Color },
            Vertex { PointB, Vector2::Zero, Vector3::Zero, Color },
        };
        ScopedPointer<VertexBuffer> VertexBuffer = VertexBuffer::Create(Points, std::size(Points), m_GraphicsApi);
        ScopedPointer<IndexBuffer> IndexBuffer = IndexBuffer::Create({0, 1}, m_GraphicsApi);

        VertexArray->SetBufferLayout(VertexBufferLayout::Default);
        
        Shader* Shader = m_Application->GetShaderManager()->Retrieve("UniformColor");
        Shader->Bind();
        

        Matrix4 Model = Matrix4::Identity;
        Model.Scale({1.0f, 1.0f, 1.0f});
        Shader->SetUniformMat4("uModel", Model);
        
        glLineWidth(Thickness);
    
        DrawIndexed(DrawMode::Lines, VertexArray, VertexBuffer, IndexBuffer, Shader);
        delete IndexBuffer;
        delete VertexBuffer;
        delete VertexArray;
    }

    void OpenGLRenderer::DrawWireQuad(const Matrix4& Transform, const Vector3& Position, const Vector2& HalfExtents, f32 Thickness, const Color& Color)
    {
        VertexArray* VertexArray = VertexArray::Create(m_GraphicsApi);
        VertexArray->Bind();
        StaticArray<Vertex, 4> Points
        {
            Vertex{ Position + Vector3(-HalfExtents.x, HalfExtents.y, 0.0f) , Vector2::Zero, Vector3::Zero, Color },
            Vertex{ Position + Vector3(+HalfExtents.x, HalfExtents.y, 0.0f) , Vector2::Zero, Vector3::Zero, Color },
            Vertex{ Position + Vector3(+HalfExtents.x, -HalfExtents.y, 0.0f) , Vector2::Zero, Vector3::Zero, Color },
            Vertex{ Position + Vector3(-HalfExtents.x, -HalfExtents.y, 0.0f) , Vector2::Zero, Vector3::Zero, Color },
        };
        VertexBuffer* VertexBuffer = VertexBuffer::Create(Points, Points.Count(), m_GraphicsApi);
        IndexBuffer* IndexBuffer = IndexBuffer::Create({0, 1, 2, 3}, m_GraphicsApi);

        VertexArray->SetBufferLayout(VertexBufferLayout::Default);
        
        Shader* Shader = m_Application->GetShaderManager()->Retrieve("UniformColor");
        Shader->Bind();
        Shader->SetUniformMat4("uModel", Transform);
        
        glLineWidth(Thickness);
    
        DrawIndexed(DrawMode::LineLoop, VertexArray, VertexBuffer, IndexBuffer, Shader);
        delete IndexBuffer;
        delete VertexBuffer;
        delete VertexArray;
    }

    void OpenGLRenderer::DrawCircle(const Matrix4& Transform, const Vector3& Position, f32 Radius, const Color& Color)
    {
        VertexArray* VertexArray = VertexArray::Create(m_GraphicsApi);
        VertexArray->Bind();
        const Vertex Points[] = {
            { Position + Vector3(-Radius, +Radius, 0.0f) , {0.0f, 1.0f}, Vector3::Zero, Color },    
            { Position + Vector3(+Radius, +Radius, 0.0f) , {1.0f, 1.0f}, Vector3::Zero, Color },
            { Position + Vector3(+Radius, -Radius, 0.0f) , {1.0f, 0.0f}, Vector3::Zero, Color },
            { Position + Vector3(-Radius, -Radius, 0.0f) , {0.0f, 0.0f}, Vector3::Zero, Color },
        };
        VertexBuffer* VertexBuffer = VertexBuffer::Create(Points, std::size(Points), m_GraphicsApi);
        IndexBuffer* IndexBuffer = IndexBuffer::Create({0, 1, 2, 3}, m_GraphicsApi);

        VertexArray->SetBufferLayout(VertexBufferLayout::Default);

        ShaderManager* Manager = m_Application->GetShaderManager();
        Shader* Shader = Manager->Retrieve("Circle");
        Shader->Bind();
        
        Shader->SetUniformMat4("uModel", Transform);
        Shader->SetUniformFloat("uThickness", 0.2f);
        Shader->SetUniformFloat("uSmoothness", 0.05f);

        DrawIndexed(DrawMode::Triangles, VertexArray, VertexBuffer, IndexBuffer, Shader);
        delete IndexBuffer;
        delete VertexBuffer;
        delete VertexArray;
    }

    void OpenGLRenderer::SetCullMode(CullMode Mode)
    {
        switch(Mode)
        {
        case CullMode::FrontFace:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            break;
        case CullMode::BackFace:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            break;
        case CullMode::FrontAndBackFaces:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT_AND_BACK);
            break;
        case CullMode::None:
            glDisable(GL_CULL_FACE);
            break;
        }
    }

    void OpenGLRenderer::SetDepthFunction(DepthFunction DepthFunction)
    {
        switch (DepthFunction)
        {
        case DepthFunction::Always:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_ALWAYS);
            break;
        case DepthFunction::Never:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_NEVER);
            break;
        case DepthFunction::Less:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            break;
        case DepthFunction::LessOrEqual:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
            break;
        case DepthFunction::Equal:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_EQUAL);
            break;
        case DepthFunction::NotEqual:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_NOTEQUAL);
            break;
        case DepthFunction::Greater:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_GREATER);
            break;
        case DepthFunction::GreaterOrEqual:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_GEQUAL);
            break;
        }
    }

    GLenum OpenGLRenderer::ConvertBlendOperation(BlendOperation Operation)
    {
        switch (Operation)
        {
        case BlendOperation::Add: return GL_FUNC_ADD;
        case BlendOperation::Min: return GL_MIN;
        case BlendOperation::Max: return GL_MAX;
        case BlendOperation::Subtract: return GL_FUNC_SUBTRACT;
        case BlendOperation::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
        }
        throw;
    }

    GLenum OpenGLRenderer::ConvertFilter(Filter Filter)
    {
        switch (Filter)
        {
        case Filter::Nearest: return GL_NEAREST;
        case Filter::Linear: return GL_LINEAR;
        default: throw;
        }
    }

    GLenum OpenGLRenderer::ConvertSamplerAddressMode(const SamplerAddressMode AddressMode)
    {
        switch (AddressMode) {
        case SamplerAddressMode::ClampToEdge: return GL_CLAMP_TO_EDGE;
        case SamplerAddressMode::Repeat: return GL_REPEAT;
        case SamplerAddressMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
        case SamplerAddressMode::ClampToBorder: GL_CLAMP_TO_BORDER;
        case SamplerAddressMode::MirrorClampToEdge: GL_MIRROR_CLAMP_TO_EDGE;
        default: throw;
        }
    }

    GLenum OpenGLRenderer::ConvertFormat(const Format Format)
    {
        switch (Format)
        {
        case Format::NONE: return GL_NONE;
        case Format::R8_UNORM: return GL_RED;
        case Format::R8_SNORM: return GL_RED;
        case Format::R16_USHORT: return GL_RED;
        case Format::R16_SHORT: return GL_RED;
        case Format::R32_FLOAT: return GL_RED;
        case Format::R32_UINT: return GL_RED;
        case Format::R32_SINT: return GL_RED;
        case Format::R8G8_UNORM: return GL_RG;
        case Format::R8G8_SNORM:return GL_RG;
        case Format::R16G16_USHORT:return GL_RG;
        case Format::R16G16_SHORT:return GL_RG;
        case Format::R32G32_UINT:return GL_RG;
        case Format::R32G32_SINT:return GL_RG;
        case Format::R32G32_FLOAT:return GL_RG;
        case Format::R8G8B8_UNORM:return GL_RGB;
        case Format::R8G8B8_SNORM:return GL_RGB;
        case Format::R16G16B16_USHORT:return GL_RGB;
        case Format::R16G16B16_SHORT:return GL_RGB;
        case Format::R32G32B32_UINT:return GL_RGB;
        case Format::R32G32B32_SINT:return GL_RGB;
        case Format::R32G32B32_FLOAT:return GL_RGB;
        case Format::R8G8B8A8_UNORM:return GL_RGBA;
        case Format::R8G8B8A8_SNORM:return GL_RGBA;
        case Format::R16G16B16A16_USHORT:return GL_RGBA;
        case Format::R16G16B16A16_SHORT:return GL_RGBA;
        case Format::R32G32B32A32_UINT:return GL_RGBA;
        case Format::R32G32B32A32_SINT:return GL_RGBA;
        case Format::R32G32B32A32_FLOAT:return GL_RGBA;
        }
        return 0;
    }

    GLenum OpenGLRenderer::GetFormatType(const Format Format)
    {
        switch (Format)
        {
        case Format::NONE: return 0;
        case Format::R8_UNORM: return GL_UNSIGNED_BYTE;
        case Format::R8_SNORM: return GL_BYTE;
        case Format::R16_USHORT: return GL_UNSIGNED_SHORT;
        case Format::R16_SHORT: return GL_SHORT;
        case Format::R32_FLOAT: return GL_FLOAT;
        case Format::R32_UINT: return GL_UNSIGNED_INT;
        case Format::R32_SINT: return GL_INT;
        case Format::R8G8_UNORM: return GL_UNSIGNED_BYTE;
        case Format::R8G8_SNORM: return GL_BYTE;
        case Format::R16G16_USHORT: return GL_UNSIGNED_SHORT;
        case Format::R16G16_SHORT: return GL_SHORT;
        case Format::R32G32_UINT: return GL_UNSIGNED_INT;
        case Format::R32G32_SINT: return GL_INT;
        case Format::R32G32_FLOAT: return GL_FLOAT;
        case Format::R8G8B8_UNORM: return GL_UNSIGNED_BYTE;
        case Format::R8G8B8_SNORM: return GL_BYTE;
        case Format::R16G16B16_USHORT: return GL_UNSIGNED_SHORT;
        case Format::R16G16B16_SHORT: return GL_SHORT;
        case Format::R32G32B32_UINT: return GL_UNSIGNED_INT;
        case Format::R32G32B32_SINT: return GL_INT;
        case Format::R32G32B32_FLOAT: return GL_FLOAT;
        case Format::R8G8B8A8_UNORM: return GL_UNSIGNED_BYTE;
        case Format::R8G8B8A8_SNORM: return GL_BYTE;
        case Format::R16G16B16A16_USHORT: return GL_UNSIGNED_SHORT;
        case Format::R16G16B16A16_SHORT: return GL_SHORT;
        case Format::R32G32B32A32_UINT: return GL_UNSIGNED_INT;
        case Format::R32G32B32A32_SINT: return GL_INT;
        case Format::R32G32B32A32_FLOAT: return GL_FLOAT;
        default: return 0;
        }
    }


    void OpenGLRenderer::SetBlendFunction(BlendMode Source, BlendMode Destination, BlendOperation Operation)
    {
        glBlendFunc(ConvertBlendMode(Source), ConvertBlendMode(Destination));
        glBlendEquation(ConvertBlendOperation(Operation));
    }

    void OpenGLRenderer::SetBlendFunction(BlendMode ColorSource, BlendMode ColorDest, BlendOperation ColorOperation, BlendMode AlphaSource, BlendMode AlphaDest, BlendOperation AlphaOperation)
    {
        glBlendFuncSeparate(ConvertBlendMode(ColorSource), ConvertBlendMode(ColorDest), ConvertBlendMode(AlphaSource), ConvertBlendMode(AlphaDest));
        glBlendEquationSeparate(ConvertBlendOperation(ColorOperation), ConvertBlendOperation(AlphaOperation));
    }
    
    void OpenGLRenderer::SetBlending(bool Enabled)
    {
        if (Enabled)
        {
            glEnable(GL_BLEND);
        } else
        {
            glDisable(GL_BLEND);
        }
    }

    void OpenGLRenderer::Blit()
    {
        
    }

    GLenum OpenGLRenderer::ConvertDrawMode(DrawMode Mode)
    {
        switch (Mode)
        {
        case DrawMode::Points: return GL_POINTS;
        case DrawMode::Lines: return GL_LINES;
        case DrawMode::LineStrip: return GL_LINE_STRIP;
        case DrawMode::Triangles: return GL_TRIANGLES;
        case DrawMode::LineLoop: return GL_LINE_LOOP;
        }
        return 0;
    }

    GLenum OpenGLRenderer::ConvertBlendMode(BlendMode Mode)
    {
        switch (Mode)
        {
        case BlendMode::Zero:                   return GL_ZERO;
        case BlendMode::One:                    return GL_ONE;
        case BlendMode::SourceColor:            return GL_SRC_COLOR;
        case BlendMode::OneMinusSourceColor:    return GL_ONE_MINUS_SRC_COLOR;
        case BlendMode::DestColor:              return GL_DST_COLOR;
        case BlendMode::OneMinusDestColor:      return GL_ONE_MINUS_DST_COLOR;
        case BlendMode::SourceAlpha:            return GL_SRC_ALPHA;
        case BlendMode::OneMinusSourceAlpha:    return GL_ONE_MINUS_SRC_ALPHA;
        case BlendMode::DestAlpha:              return GL_DST_ALPHA;
        case BlendMode::OneMinusDestAlpha:      return GL_ONE_MINUS_DST_ALPHA;
        case BlendMode::ConstantColor:          return GL_CONSTANT_COLOR;
        case BlendMode::OnMinusConstantColor:   return GL_ONE_MINUS_CONSTANT_COLOR;
        case BlendMode::ConstantAlpha:          return GL_CONSTANT_ALPHA;
        case BlendMode::OneMinusConstantAlpha:  return GL_ONE_MINUS_CONSTANT_ALPHA;
        case BlendMode::SourceAlphaSaturated:   return GL_SRC_ALPHA_SATURATE;
        case BlendMode::Source1Color:           return GL_SRC1_COLOR;
        case BlendMode::OneMinusSource1Color:   return GL_ONE_MINUS_SRC1_COLOR;
        case BlendMode::Source1Alpha:           return GL_SRC1_ALPHA;
        case BlendMode::OneMinusSource1Alpha:   return GL_ONE_MINUS_SRC1_ALPHA;
        }
        return -1u;
    }


    String OpenGLRenderer::GetDebugSourceName(u32 Source)
    {
        switch (Source)
        {
        case GL_DEBUG_SOURCE_API : return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:	return "WINDOW SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
        case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER: return "OTHER";
        default: return "UNKNOWN";
        }
    }

    Verbosity OpenGLRenderer::GetDebugVerbosity(u32 Severity)
    {
        switch (Severity)
        {
        case GL_DEBUG_SEVERITY_HIGH: return Verbosity::Error;
        case GL_DEBUG_SEVERITY_MEDIUM:
        case GL_DEBUG_SEVERITY_LOW: return Verbosity::Warning;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
        default: return Verbosity::Trace;
        }
    }
}

