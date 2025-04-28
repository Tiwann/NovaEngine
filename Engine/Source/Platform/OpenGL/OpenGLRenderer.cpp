#include "OpenGLRenderer.h"
#include "Runtime/Application.h"
#include "Runtime/Window.h"
#include "Runtime/Log.h"
#include "Runtime/Color.h"
#include "Runtime/LogVerbosity.h"
#include "Components/Camera.h"
#include "Rendering/Shader.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/VertexArray.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/VertexLayout.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "OpenGLPipeline.h"
#include "OpenGLShader.h"
#include "Rendering/Pipeline.h"


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


        const auto Callback = [](const GLenum Source, GLenum, GLuint, const GLenum Severity, GLsizei, const GLchar *Message, const void *) -> void
        {
            const Verbosity Verbo = GetDebugVerbosity(Severity);
            const String SourceName = GetDebugSourceName(Source);
            if(Verbo < Verbosity::Warning)
                return;
            NOVA_LOG(OpenGL, Verbo, "Debug ({}): {}", SourceName, Message);
        };

#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(Callback, nullptr);
#endif
        

        const String Renderer = (const char*)glGetString(GL_RENDERER);
        NOVA_LOG(OpenGL, Verbosity::Info, "Using OpenGL 4.6");
        NOVA_LOG(OpenGL, Verbosity::Info, "Using GPU: {}", Renderer);
        return true;
    }

    void OpenGLRenderer::Destroy()
    {
    }

    void OpenGLRenderer::ClearDepth(const float Depth)
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

    void OpenGLRenderer::BeginRendering()
    {
    }

    void OpenGLRenderer::EndRendering()
    {
    }

    void OpenGLRenderer::EndFrame()
    {
    }

    void OpenGLRenderer::SetViewportRect(const Vector2 Position, const Vector2 Size)
    {
        glViewport((GLint)Position.x, (GLint)Position.y, (GLint)Size.x, (GLint)Size.y);
    }

    void OpenGLRenderer::Draw(VertexArray* VertexArray, u32 NumVert, Shader* Shader)
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

        //glDrawArrays(ConvertPolygonMode(Mode), 0, (i32)NumVert);
    }

    void OpenGLRenderer::DrawIndexed(VertexBuffer* VertexBuffer, IndexBuffer* IndexBuffer)
    {
        VertexBuffer->Bind();
        IndexBuffer->Bind();
        const PrimitiveTopology& Topology = m_BoundPipeline->GetSpecification().PrimitiveTopology;
        const GLenum GLTopology = Convertor.ConvertPrimitiveTopology(Topology);
        glDrawElements(GLTopology, (GLsizei)IndexBuffer->Count(), GL_UNSIGNED_INT, nullptr);
    }
    
    void OpenGLRenderer::SetCullMode(const CullMode Mode)
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

    void OpenGLRenderer::SetDepthCompareOperation(const CompareOperation CompareOperation)
    {
        switch (CompareOperation)
        {
        case CompareOperation::Always:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_ALWAYS);
            break;
        case CompareOperation::Never:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_NEVER);
            break;
        case CompareOperation::Less:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            break;
        case CompareOperation::LessOrEqual:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
            break;
        case CompareOperation::Equal:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_EQUAL);
            break;
        case CompareOperation::NotEqual:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_NOTEQUAL);
            break;
        case CompareOperation::Greater:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_GREATER);
            break;
        case CompareOperation::GreaterOrEqual:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_GEQUAL);
            break;
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


    void OpenGLRenderer::SetBlendFunction(const BlendFactor Source, const BlendFactor Destination, const BlendOperation Operation)
    {
        glBlendFunc(Convertor.ConvertBlendFactor(Source), Convertor.ConvertBlendFactor(Destination));
        glBlendEquation(Convertor.ConvertBlendOperation(Operation));
    }

    void OpenGLRenderer::SetBlendFunction(const BlendFactor ColorSource, const BlendFactor ColorDest, const BlendOperation ColorOperation, const BlendFactor AlphaSource, const BlendFactor
                                          AlphaDest, const BlendOperation AlphaOperation)
    {
        glBlendFuncSeparate(Convertor.ConvertBlendFactor(ColorSource), Convertor.ConvertBlendFactor(ColorDest), Convertor.ConvertBlendFactor(AlphaSource), Convertor.ConvertBlendFactor(AlphaDest));
        glBlendEquationSeparate(Convertor.ConvertBlendOperation(ColorOperation), Convertor.ConvertBlendOperation(AlphaOperation));
    }
    
    void OpenGLRenderer::SetBlending(const bool Enabled)
    {
        if (Enabled)
        {
            glEnable(GL_BLEND);
        } else
        {
            glDisable(GL_BLEND);
        }
    }

    static void SetFeatureEnabled(const GLenum Feature, const bool Enabled)
    {
        const Function<void(GLenum)> EnableFunc = Enabled ? glEnable : glDisable;
        EnableFunc.Call(Feature);
    }

    void OpenGLRenderer::BindPipeline(Pipeline* Pipeline)
    {
        const PipelineSpecification& Specification = Pipeline->GetSpecification();
        OpenGLPipeline* CastedPipeline = Pipeline->As<OpenGLPipeline>();
        m_BoundPipeline = CastedPipeline;
        const u32 VertexArrayObject = CastedPipeline->GetVertexArrayObject();
        glBindVertexArray(VertexArrayObject);
        glBindVertexArray(VertexArrayObject);
        for (size_t i = 0; i < Specification.VertexLayout.Count(); i++)
        {
            const VertexAttribute& Attribute = Specification.VertexLayout[i];
            const size_t Count = GetFormatComponentCount(Attribute.Format);
            glEnableVertexAttribArray((GLuint)i);
            glVertexAttribPointer((GLuint)i, (GLint)Count, GL_FLOAT, GL_FALSE, (GLsizei)Specification.VertexLayout.Stride(), (const void*)Specification.VertexLayout.GetOffset(Attribute));
        }


        SetFeatureEnabled(GL_DEPTH_TEST, Specification.DepthTestEnable);
        SetFeatureEnabled(GL_DEPTH_CLAMP, Specification.DepthClampEnable);
        SetFeatureEnabled(GL_STENCIL_TEST, Specification.StencilTestEnable);

        glDepthFunc(Convertor.ConvertCompareOperation(Specification.DepthCompareOperation));
        glDepthRange(Specification.Viewport.MinDepth, Specification.Viewport.MaxDepth);


        glLineWidth(Specification.LineWidth);

        const Viewport& Viewport = Specification.Viewport;
        const Scissor& Scissor = Specification.Scissor;
        glViewport(Viewport.X, Viewport.Y, Viewport.Width, Viewport.Height);
        glScissor(Scissor.X, Scissor.Y, Scissor.Width, Scissor.Height);


        SetFeatureEnabled(GL_CULL_FACE, Specification.CullMode != CullMode::None);
        if (Specification.CullMode != CullMode::None)
            glCullFace(Convertor.ConvertCullMode(Specification.CullMode));
        glFrontFace(Convertor.ConvertFrontFace(Specification.FrontFace));

        SetFeatureEnabled(GL_BLEND, Specification.BlendEnable);
        if (Specification.BlendEnable)
        {
            const auto& [ColorSource, ColorDest, ColorOperation, AlphaSource, AlphaDest, AlphaOperation] = Specification.BlendFunction;
            glBlendFuncSeparate(Convertor.ConvertBlendFactor(ColorSource), Convertor.ConvertBlendFactor(ColorDest), Convertor.ConvertBlendFactor(AlphaSource), Convertor.ConvertBlendFactor(AlphaDest));
            glBlendEquationSeparate(Convertor.ConvertBlendOperation(ColorOperation), Convertor.ConvertBlendOperation(AlphaOperation));
        }

        SetFeatureEnabled(GL_SAMPLE_ALPHA_TO_COVERAGE, Specification.AlphaToCoverageEnable);
        SetFeatureEnabled(GL_SAMPLE_ALPHA_TO_ONE, Specification.AlphaToOneEnable);
        SetFeatureEnabled(GL_MULTISAMPLE, Specification.MultisampleEnable);

        glPolygonMode(GL_FRONT_AND_BACK, Convertor.ConvertPolygonMode(Specification.PolygonMode));

        Specification.ShaderProgram->Bind();
    }

    void OpenGLRenderer::UpdateUniformBuffer(UniformBuffer* Buffer, u64 Offset, u64 Size, const void* Data)
    {
    }

    String OpenGLRenderer::GetDebugSourceName(const u32 Source)
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

    Verbosity OpenGLRenderer::GetDebugVerbosity(const u32 Severity)
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

