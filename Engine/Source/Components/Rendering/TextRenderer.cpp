#include "TextRenderer.h"

#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/Pipeline.h"
#include "Rendering/Texture2D.h"
#include "Rendering/VertexBuffer.h"
#include "ResourceManager/ShaderManager.h"
#include "Runtime/Application.h"
#include "Runtime/Entity.h"
#include "Runtime/Font.h"
#include "Runtime/Scene.h"
#include "Runtime/Utils.h"
#include "Runtime/Window.h"

#include <vulkan/vulkan.h>

#include "Platform/Vulkan/VulkanRenderer.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanTexture2D.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"

namespace Nova
{
    struct SceneData
    {
        Matrix4 ModelMatrix;
        Matrix4 ViewMatrix;
        Matrix4 ProjectionMatrix;
    };

    void TextRenderer::OnInit()
    {

        OnTextChangedEvent.BindMember(this, &TextRenderer::OnTextChanged);

        const Entity* Owner = GetOwner();
        const Scene* Scene = Owner->GetOwner();
        const Application* Application = Scene->GetOwner();
        const Window* Window = Application->GetWindow();
        Renderer* Renderer = Application->GetRenderer();

        const i32 Width = Window->GetWidth();
        const i32 Height = Window->GetHeight();

        ShaderManager* ShaderManager = Application->GetShaderManager();
        Shader* FontShader = ShaderManager->Retrieve("Font");

        PipelineCreateInfo PipelineCreateInfo;
        PipelineCreateInfo.VertexLayout.AddAttribute({"POSITION", Format::Vector3});
        PipelineCreateInfo.VertexLayout.AddAttribute({"TEXCOORDINATE", Format::Vector2});
        PipelineCreateInfo.VertexLayout.AddAttribute({"NORMAL", Format::Vector3});
        PipelineCreateInfo.VertexLayout.AddAttribute({"COLOR", Format::Vector4});
        PipelineCreateInfo.BlendEnable = false;
        PipelineCreateInfo.CullMode = CullMode::BackFace;
        PipelineCreateInfo.FrontFace = FrontFace::CounterClockwise;
        PipelineCreateInfo.Viewport = Viewport(0.0f, 0.0f, Width, Height, 0.0f, 1.0f);
        PipelineCreateInfo.Scissor = Scissor(0, 0, Width, Height);
        PipelineCreateInfo.PolygonMode = PolygonMode::Fill;
        PipelineCreateInfo.PrimitiveTopology = PrimitiveTopology::TriangleList;
        PipelineCreateInfo.DepthBiasEnable = false;
        PipelineCreateInfo.DepthClampEnable = false;
        PipelineCreateInfo.DepthTestEnable = true;
        PipelineCreateInfo.DepthWriteEnable = true;
        PipelineCreateInfo.DepthCompareOperation = CompareOperation::Less;
        PipelineCreateInfo.PrimitiveRestartEnable = false;
        PipelineCreateInfo.RasterizerDiscardEnable = false;
        PipelineCreateInfo.StencilTestEnable = false;
        PipelineCreateInfo.DynamicRendering = false;
        PipelineCreateInfo.ShaderProgram = FontShader;
        PipelineCreateInfo.RasterizationSamples = SampleCount::S8;
        PipelineCreateInfo.MultisampleEnable = true;

        m_Pipeline = Renderer->CreatePipeline(PipelineCreateInfo);


        m_VertexBuffer = Renderer->CreateVertexBuffer(VertexBufferCreateInfo(nullptr, 0));
        m_IndexBuffer = Renderer->CreateIndexBuffer(IndexBufferCreateInfo(Format::None, nullptr, 0));
        m_UniformBuffer = Renderer->CreateUniformBuffer(sizeof(SceneData));

        if (Renderer->GetGraphicsApi() == GraphicsApi::Vulkan)
        {
            const VkDevice Device = Renderer->As<VulkanRenderer>()->GetDevice();
            const Array<VkDescriptorSet>& Sets = FontShader->As<VulkanShader>()->GetDescriptorSets();
            const Texture2D* FontAtlasTexture = m_Font->GetAtlasTexture();

            const VkDescriptorImageInfo ImageInfo
            {
                .sampler = FontAtlasTexture->As<VulkanTexture2D>()->GetSampler(),
                .imageView = FontAtlasTexture->As<VulkanTexture2D>()->GetImageView(),
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };

            VkDescriptorBufferInfo BufferInfo { };
            BufferInfo.buffer = m_UniformBuffer->As<VulkanUniformBuffer>()->GetHandle();
            BufferInfo.offset = 0;
            BufferInfo.range = VK_WHOLE_SIZE;

            VkWriteDescriptorSet WriteDescriptors[2] { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };

            WriteDescriptors[0].descriptorCount = 1;
            WriteDescriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            WriteDescriptors[0].dstSet = Sets[0];
            WriteDescriptors[0].dstBinding = 0;
            WriteDescriptors[0].pBufferInfo = &BufferInfo;

            WriteDescriptors[1].descriptorCount = 1;
            WriteDescriptors[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            WriteDescriptors[1].dstSet = Sets[0];
            WriteDescriptors[1].dstBinding = 1;
            WriteDescriptors[1].pImageInfo = &ImageInfo;

            vkUpdateDescriptorSets(Device, ArrayCount(WriteDescriptors), WriteDescriptors, 0, nullptr);
        }
    }

    void TextRenderer::OnDestroy()
    {
        Component::OnDestroy();
        m_VertexBuffer->Destroy();
        delete m_VertexBuffer;
        m_IndexBuffer->Destroy();
        delete m_IndexBuffer;
    }

    void TextRenderer::OnUpdate(f32 Delta)
    {
        Component::OnUpdate(Delta);
        if (m_Text != m_LastText)
        {
            if (OnTextChangedEvent.IsBound())
                OnTextChangedEvent.Broadcast(m_Text);
        }
    }

    void TextRenderer::OnFrameBegin(Renderer* Renderer)
    {
        Component::OnFrameBegin(Renderer);

        Camera* Camera = Renderer->GetCurrentCamera();
        if (!Camera) return;

        const Entity* Entity = GetOwner();

        Transform* EntityTransform = Entity->GetTransform();

        const SceneData SceneDataInstance
        {
            EntityTransform->GetWorldSpaceMatrix(),
            Camera->GetViewMatrix(),
            Camera->GetProjectionMatrix()
        };

        Renderer->UpdateUniformBuffer(m_UniformBuffer, 0, sizeof(SceneData), &SceneDataInstance);
    }

    void TextRenderer::OnRender(Renderer* Renderer)
    {
        Component::OnRender(Renderer);

        Entity* Entity = GetOwner();
        const Scene* Scene = Entity->GetOwner();
        const Application* Application = Scene->GetOwner();
        const Window* Window = Application->GetWindow();
        const f32 Width = Window->GetWidth();
        const f32 Height = Window->GetHeight();

        Renderer->BindPipeline(m_Pipeline);
        Renderer->SetViewport(Viewport(0.0f, 0.0f, Width, Height, 0.0f, 1.0f));
        Renderer->SetScissor(Scissor(0, 0, (i32)Width, (i32)Height));

        Renderer->BindVertexBuffer(m_VertexBuffer, 0);
        Renderer->BindIndexBuffer(m_IndexBuffer, 0);
        Renderer->DrawIndexed(m_IndexBuffer->GetCount(), 0);
    }

    void TextRenderer::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);

        static char Buffer[256] = {0};
        if (ImGui::InputTextMultiline("Text", Buffer, ArrayCount(Buffer), ImVec2(0, 0),
                                      ImGuiInputTextFlags_EnterReturnsTrue))
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

    void TextRenderer::OnTextChanged(const String& NewText)
    {
        Array<Vertex> Vertices;
        Array<u32> Indices;
        GetTextQuads(Vertices, Indices);

        m_VertexBuffer->Initialize(VertexBufferCreateInfo(Vertices.Data(), Vertices.Count()));
        m_IndexBuffer->Initialize(IndexBufferCreateInfo(Format::R32_UINT, Indices.Data(), Indices.Count()));
    }

    void TextRenderer::GetTextQuads(Array<Vertex>& OutVertices, Array<u32>& OutIndices)
    {
        const auto& FontData = m_Font->GetFontData();
        const auto& FontGeometry = FontData.FontGeometry;
        const auto& Metrics = FontGeometry.getMetrics();
        const Texture2D* FontAtlasTexture = m_Font->GetAtlasTexture();

        OutVertices.Clear();
        OutIndices.Clear();

        f64 X = 0.0;
        const f64 FsScale = 1.0 / (Metrics.ascenderY - Metrics.descenderY);
        f64 Y = 0.0;

        const f32 SpaceGlyphAdvance = FontGeometry.getGlyph(' ')->getAdvance();

        for (size_t CharacterIndex = 0; CharacterIndex < m_Text.Count(); CharacterIndex++)
        {
            const String::CharacterType Character = m_Text[CharacterIndex];

            if (Character == '\r')
                continue;

            if (Character == '\n')
            {
                X = 0;
                Y -= FsScale * Metrics.lineHeight; // + TextParams.LineSpacing
                continue;
            }

            if (Character == ' ')
            {
                f32 Advance = SpaceGlyphAdvance;
                if (CharacterIndex < m_Text.Count() - 1)
                {
                    const String::CharacterType NextCharacter = m_Text[CharacterIndex + 1];
                    f64 DAdvance;
                    FontGeometry.getAdvance(DAdvance, Character, NextCharacter);
                    Advance = static_cast<f32>(DAdvance);
                }

                X += FsScale * Advance; // + TextParams.Kerning
                continue;
            }

            if (Character == '\t')
            {
                X += 4.0f * (FsScale * SpaceGlyphAdvance); // + TextParams.Kerning
                continue;
            }

            auto Glyph = FontGeometry.getGlyph(Character);
            if (!Glyph)
                Glyph = FontGeometry.getGlyph('?');

            if (!Glyph)
                continue;

            f64 Al, Ab, Ar, At;
            Glyph->getQuadAtlasBounds(Al, Ab, Ar, At);
            Vector2 TexCoordMin(Al, Ab);
            Vector2 TexCoordMax(Ar, At);

            f64 Pl, Pb, Pr, Pt;
            Glyph->getQuadPlaneBounds(Pl, Pb, Pr, Pt);
            Vector2 QuadMin(Pl, Pb);
            Vector2 QuadMax(Pr, Pt);

            QuadMin *= FsScale;
            QuadMax *= FsScale;
            QuadMin += Vector2(X, Y);
            QuadMax += Vector2(X, Y);

            const f32 TexelWidth = 1.0f / FontAtlasTexture->GetWidth();
            const f32 TexelHeight = 1.0f / FontAtlasTexture->GetHeight();
            TexCoordMin *= Vector2(TexelWidth, TexelHeight);
            TexCoordMax *= Vector2(TexelWidth, TexelHeight);

            OutVertices.AddRange({
                Vertex({QuadMin, 0.0f}, TexCoordMin, Vector3::Zero, Color::White),
                Vertex({QuadMin.x, QuadMax.y, 0.0f}, {TexCoordMin.x, TexCoordMax.y}, Vector3::Zero, Color::White),
                Vertex({QuadMax, 0.0f}, TexCoordMax, Vector3::Zero, Color::White),
                Vertex({QuadMax.x, QuadMin.y, 0.0f}, {TexCoordMax.x, TexCoordMin.y}, Vector3::Zero, Color::White),
            });

            OutIndices.AddRange({
                0 + 4 * (u32)CharacterIndex, 2 + 4 * (u32)CharacterIndex, 1 + 4 * (u32)CharacterIndex, 0 + 4 * (u32)CharacterIndex, 3 + 4 * (u32)CharacterIndex, 2 + 4 * (u32)CharacterIndex
            });

            if (CharacterIndex < m_Text.Count() - 1)
            {
                f64 Advance = Glyph->getAdvance();
                const char NextCharacter = m_Text[CharacterIndex + 1];
                FontGeometry.getAdvance(Advance, Character, NextCharacter);

                X += FsScale * Advance; // + TextParams.Kerning
            }
        }
    }
}
