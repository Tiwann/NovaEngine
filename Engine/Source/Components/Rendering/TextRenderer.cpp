#include "TextRenderer.h"

#include "Rendering/Pipeline.h"
#include "ResourceManager/ShaderManager.h"
#include "Runtime/Application.h"
#include "Runtime/Entity.h"
#include "Runtime/Font.h"
#include "Runtime/Scene.h"
#include "Runtime/Utils.h"
#include "Runtime/Window.h"

namespace Nova
{
    void TextRenderer::OnInit()
    {
        const Entity* Owner = GetOwner();
        const Scene* Scene = Owner->GetScene();
        const Application* Application = Scene->GetOwner();
        const Window* Window = Application->GetWindow();

        const i32 Width = Window->GetWidth();
        const i32 Height = Window->GetHeight();

        ShaderManager* ShaderManager = Application->GetShaderManager();
        Shader* FontShader = ShaderManager->Retrieve("Font");

        PipelineCreateInfo PipelineSpecification;
        PipelineSpecification.VertexLayout.AddAttribute({"POSITION", Format::Vector3});
        PipelineSpecification.VertexLayout.AddAttribute({"TEXCOORDINATE", Format::Vector2});
        PipelineSpecification.VertexLayout.AddAttribute({"COLOR", Format::Vector4});
        PipelineSpecification.BlendEnable = false;
        PipelineSpecification.CullMode = CullMode::BackFace;
        PipelineSpecification.FrontFace = FrontFace::CounterClockwise;
        PipelineSpecification.Viewport = Viewport(0.0f, 0.0f, Width, Height, 0.0f, 1.0f);
        PipelineSpecification.Scissor = Scissor(0, 0, Width, Height);
        PipelineSpecification.PolygonMode = PolygonMode::Fill;
        PipelineSpecification.PrimitiveTopology = PrimitiveTopology::TriangleList;
        PipelineSpecification.DepthBiasEnable = false;
        PipelineSpecification.DepthClampEnable = false;
        PipelineSpecification.DepthTestEnable = true;
        PipelineSpecification.DepthWriteEnable = true;
        PipelineSpecification.DepthCompareOperation = CompareOperation::Less;
        PipelineSpecification.PrimitiveRestartEnable = false;
        PipelineSpecification.RasterizerDiscardEnable = false;
        PipelineSpecification.StencilTestEnable = false;
        PipelineSpecification.DynamicRendering = false;
        PipelineSpecification.ShaderProgram = FontShader;
        PipelineSpecification.RasterizationSamples = SampleCount::S8;
        PipelineSpecification.MultisampleEnable = true;

    }

    void TextRenderer::OnRender(Renderer* Renderer)
    {
        Component::OnRender(Renderer);

        const auto& FontData = m_Font->GetFontData();
        const auto& FontGeometry = FontData->FontGeometry;

    }

    void TextRenderer::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);

        static char Buffer[256] = { 0 };
        if (ImGui::InputTextMultiline("Text", Buffer, ArrayCount(Buffer), ImVec2(0, 0), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            m_Text = String(Buffer);
        }

    }

    void TextRenderer::SetText(const String& NewText)
    {
        m_Text = NewText;
    }

    const String& TextRenderer::GetText() const
    {
        return m_Text;
    }

    void TextRenderer::SetFont(Font* Font)
    {
        m_Font = Font;
    }

    Font* TextRenderer::GetFont() const
    {
        return m_Font;
    }

    void TextRenderer::SetTextAlignment(TextAlignment NewTextAlignment)
    {
        m_TextAlignment = NewTextAlignment;
    }

    TextAlignment TextRenderer::GetTextAlignment() const
    {
        return m_TextAlignment;
    }

    void TextRenderer::SetTextStyle(TextStyleFlags NewTextStyleFlags)
    {
        m_StyleFlags = NewTextStyleFlags;
    }

    TextStyleFlags TextRenderer::GetTextStyle() const
    {
        return m_StyleFlags;
    }
}
