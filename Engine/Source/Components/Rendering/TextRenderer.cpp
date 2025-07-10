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

#include "Editor/Font.h"
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanTexture2D.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"
#include "Runtime/AssetDatabase.h"
#include "Editor/EditorGUI.h"
#include "Platform/Vulkan/VulkanCommandBuffer.h"

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
        AssetDatabase* AssetDatabase = g_Application->GetAssetDatabase();
        m_Font = AssetDatabase->CreateAsset<Font>("Font");
        const Path FontPath = GetFontPath(OpenSans_ExtraBold);
        const Array<CharacterSetRange> CharacterSetRanges = { { 0x0020, 0x00FF }};
        const FontParams FontParams { FontAtlasType::MTSDF, { ArrayView(CharacterSetRanges) }};
        if (!m_Font->LoadFromFile(FontPath, FontParams))
        {
            NOVA_BREAK();
            return;
        }

        const Entity* Owner = GetOwner();
        const Scene* Scene = Owner->GetOwner();
        const Application* Application = Scene->GetOwner();
        const Window* Window = Application->GetWindow();
        Renderer* Renderer = Application->GetRenderer();

        const i32 Width = Window->GetWidth();
        const i32 Height = Window->GetHeight();

        ShaderManager* ShaderManager = Application->GetShaderManager();
        Shader* FontShader = ShaderManager->Retrieve("Font");

        if (Renderer->GetGraphicsApi() == GraphicsApi::Vulkan)
        {
            VulkanShader* Shader = FontShader->As<VulkanShader>();
            m_PipelineLayout = FontShader->As<VulkanShader>()->CreatePipelineLayout(Shader->GetDescriptorSetLayouts(), Array<VkPushConstantRange>());
        }

        PipelineCreateInfo PipelineCreateInfo;
        PipelineCreateInfo.VertexLayout.AddAttribute({"POSITION", Format::Vector3});
        PipelineCreateInfo.VertexLayout.AddAttribute({"TEXCOORDINATE", Format::Vector2});
        PipelineCreateInfo.VertexLayout.AddAttribute({"NORMAL", Format::Vector3});
        PipelineCreateInfo.VertexLayout.AddAttribute({"COLOR", Format::Vector4});
        PipelineCreateInfo.BlendEnable = true;
        PipelineCreateInfo.BlendFunction = AlphaBlend;

        PipelineCreateInfo.CullMode = CullMode::BackFace;
        PipelineCreateInfo.FrontFace = FrontFace::CounterClockwise;
        PipelineCreateInfo.Viewport = Viewport(0.0f, 0.0f, Width, Height, 0.0f, 1.0f);
        PipelineCreateInfo.Scissor = Scissor(0, 0, Width, Height);
        PipelineCreateInfo.PolygonMode = PolygonMode::Fill;
        PipelineCreateInfo.PrimitiveTopology = PrimitiveTopology::TriangleList;
        PipelineCreateInfo.DepthBiasEnable = false;
        PipelineCreateInfo.DepthClampEnable = false;
        PipelineCreateInfo.DepthTestEnable = false;
        PipelineCreateInfo.DepthWriteEnable = false;
        PipelineCreateInfo.DepthCompareOperation = CompareOperation::Less;
        PipelineCreateInfo.PrimitiveRestartEnable = false;
        PipelineCreateInfo.RasterizerDiscardEnable = false;
        PipelineCreateInfo.StencilTestEnable = false;
        PipelineCreateInfo.DynamicRendering = false;
        PipelineCreateInfo.ShaderProgram = FontShader;
        PipelineCreateInfo.RasterizationSamples = SampleCount::S8;
        PipelineCreateInfo.MultisampleEnable = true;
        PipelineCreateInfo.VulkanPipelineLayout = m_PipelineLayout;

        m_Pipeline = Renderer->CreatePipeline(PipelineCreateInfo);
        m_VertexBuffer = Renderer->CreateVertexBuffer(VertexBufferCreateInfo(nullptr, 0));
        m_IndexBuffer = Renderer->CreateIndexBuffer(IndexBufferCreateInfo(Format::None, nullptr, 0));
        m_UniformBuffer = Renderer->CreateUniformBuffer(sizeof(SceneData));

        if (Renderer->GetGraphicsApi() == GraphicsApi::Vulkan)
        {
            VulkanShader* Shader = FontShader->As<VulkanShader>();
            m_DescriptorSets = Shader->AllocateDescriptorSets();

            const VkDevice Device = Renderer->As<VulkanRenderer>()->GetDevice();
            const Texture2D* FontAtlasTexture = m_Font->GetAtlasTexture();

            const VkDescriptorImageInfo ImageInfo
            {
                .sampler = FontAtlasTexture->As<VulkanTexture2D>()->GetSampler(),
                .imageView = FontAtlasTexture->As<VulkanTexture2D>()->GetImageView(),
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };

            const VkDescriptorBufferInfo BufferInfo = m_UniformBuffer->As<VulkanUniformBuffer>()->GetDescriptorInfo(0);


            VkWriteDescriptorSet WriteDescriptors[2] { { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET }, { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET } };

            WriteDescriptors[0].descriptorCount = 1;
            WriteDescriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            WriteDescriptors[0].dstSet = m_DescriptorSets.First();
            WriteDescriptors[0].dstBinding = 0;
            WriteDescriptors[0].pBufferInfo = &BufferInfo;

            WriteDescriptors[1].descriptorCount = 1;
            WriteDescriptors[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            WriteDescriptors[1].dstSet = m_DescriptorSets.First();
            WriteDescriptors[1].dstBinding = 1;
            WriteDescriptors[1].pImageInfo = &ImageInfo;

            vkUpdateDescriptorSets(Device, ArrayCount(WriteDescriptors), WriteDescriptors, 0, nullptr);
        }
        m_ResourcesDirty = true;
    }

    void TextRenderer::OnDestroy()
    {
        Component::OnDestroy();
        m_VertexBuffer->Destroy();
        delete m_VertexBuffer;
        m_IndexBuffer->Destroy();
        delete m_IndexBuffer;
        delete m_Font;
    }

    void TextRenderer::OnUpdate(f32 Delta)
    {
        Component::OnUpdate(Delta);
        if (m_ResourcesDirty)
        {
            UpdateResources();
            m_ResourcesDirty = false;
        }
    }

    void TextRenderer::OnFrameBegin(Renderer* Renderer)
    {
        Component::OnFrameBegin(Renderer);
    }

    void TextRenderer::OnRender(Renderer* Renderer)
    {
        Component::OnRender(Renderer);

        if (m_Text.IsEmpty())
            return;

        if (!m_Font)
            return;

        Entity* Entity = GetOwner();
        const Scene* Scene = Entity->GetOwner();
        const Application* Application = Scene->GetOwner();
        const Window* Window = Application->GetWindow();
        const f32 Width = Window->GetWidth();
        const f32 Height = Window->GetHeight();

        const VulkanCommandBuffer* Cmd = Renderer->As<VulkanRenderer>()->GetCurrentCommandBuffer();

        Camera* Camera = Renderer->GetCurrentCamera();
        if (!Camera) return;

        Transform* EntityTransform = Entity->GetTransform();


        const Matrix4& WorldSpaceMatrix = EntityTransform->GetWorldSpaceMatrix();
        
        const SceneData SceneDataInstance
        {
            WorldSpaceMatrix,
            Camera->GetViewMatrix(),
            Camera->GetProjectionMatrix()
        };

        Renderer->UpdateUniformBuffer(m_UniformBuffer, 0, sizeof(SceneData), &SceneDataInstance);


        Renderer->BindPipeline(m_Pipeline);
        vkCmdBindDescriptorSets(Cmd->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, m_DescriptorSets.Count(), m_DescriptorSets.Data(), 0, nullptr);

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

        Memory::Memzero(Buffer);
        memcpy(Buffer, m_Text.Data(), m_Text.Size());

        if (ImGui::InputTextMultiline("Text", Buffer, ArrayCount(Buffer), ImVec2(0, 0), 0, nullptr, this))
        {
            SetText(Buffer);
        }

        if (UI::DragValue("Line spacing", m_LineSpacing))
        {
            m_ResourcesDirty = true;
        }

        if (UI::DragValue("Character Spacing", m_CharacterSpacing))
        {
            m_ResourcesDirty = true;
        }

        const char* const Names[] = { "Left", "Center", "Right" };
        if (ImGui::Combo("Alignment", (int*)&m_TextAlignment, Names, ArrayCount(Names)))
        {
            m_ResourcesDirty = true;
        }
    }

    void TextRenderer::SetText(const String& NewText)
    {
        m_Text = NewText;
        m_ResourcesDirty = true;
    }

    const String& TextRenderer::GetText() const
    {
        return m_Text;
    }

    void TextRenderer::SetFont(Font* Font)
    {
        m_Font = Font;
        m_ResourcesDirty = true;
    }

    Font* TextRenderer::GetFont() const
    {
        return m_Font;
    }

    void TextRenderer::SetTextAlignment(TextAlignment NewTextAlignment)
    {
        m_TextAlignment = NewTextAlignment;
        m_ResourcesDirty = true;
    }

    TextAlignment TextRenderer::GetTextAlignment() const
    {
        return m_TextAlignment;
    }

    void TextRenderer::SetTextStyle(TextStyleFlags NewTextStyleFlags)
    {
        m_StyleFlags = NewTextStyleFlags;
        m_ResourcesDirty = true;
    }

    TextStyleFlags TextRenderer::GetTextStyle() const
    {
        return m_StyleFlags;
    }

    float TextRenderer::CalculateTextWidth() const
    {
        using namespace msdf_atlas;
        const FontData& FontData = m_Font->GetFontData();
        const FontGeometry& FontGeometry = FontData.FontGeometry;

        float Result = 0.0f;
        for (size_t Index = 0; Index < m_Text.Count(); Index++)
        {
            const String::CharacterType Character = m_Text[Index];
            const GlyphGeometry* Glyph = FontGeometry.getGlyph(Character) ? FontGeometry.getGlyph(Character) : FontGeometry.getGlyph('?') ? FontGeometry.getGlyph('?') : nullptr;

            if (Character == '\t')
            {
                Result += 4.0f * FontGeometry.getGlyph(' ')->getAdvance();
                continue;
            }

            if (Index != m_Text.Count() - 1)
            {
                const String::CharacterType NextCharacter = m_Text[Index + 1];
                f64 Advance = 0.0;
                FontGeometry.getAdvance(Advance, Character, NextCharacter);
                Result += Advance + m_CharacterSpacing;
            }
        }

        return Result;
    }

    void TextRenderer::UpdateResources()
    {
        if (m_Text.IsEmpty())
            return;

        Array<Vertex> Vertices;
        Array<u32> Indices;
        GetTextQuads(Vertices, Indices);

        m_VertexBuffer->Initialize(VertexBufferCreateInfo(Vertices.Data(), Vertices.Count()));
        m_IndexBuffer->Initialize(IndexBufferCreateInfo(Format::R32_UINT, Indices.Data(), Indices.Size()));
    }

    void TextRenderer::GetTextQuads(Array<Vertex>& OutVertices, Array<u32>& OutIndices)
    {
        using namespace msdf_atlas;
        const auto& FontData = m_Font->GetFontData();
        const auto& FontGeometry = FontData.FontGeometry;
        const auto& Metrics = FontGeometry.getMetrics();

        OutVertices.Clear();
        OutIndices.Clear();

        f32 PositionX = 0.0f, PositionY = 0.0f;

        switch (m_TextAlignment)
        {
        case TextAlignment::Left:
            PositionX = 0.0f;
            break;
        case TextAlignment::Center:
            PositionX = -0.5f * CalculateTextWidth();
            break;
        case TextAlignment::Right:
            PositionX = CalculateTextWidth();
            break;
        }

        for (size_t Index = 0; Index < m_Text.Count(); Index++)
        {
            const String::CharacterType Character = m_Text[Index];

            const GlyphGeometry* Glyph = FontGeometry.getGlyph(Character) ? FontGeometry.getGlyph(Character) : FontGeometry.getGlyph('?') ? FontGeometry.getGlyph('?') : nullptr;
            if (!Glyph) continue;

            if (Character == '\r')
                continue;


            if (Character == '\t')
            {
                PositionX += 4.0f * FontGeometry.getGlyph(' ')->getAdvance();
                continue;
            }

            if (Character == '\n')
            {
                PositionX = 0.0f;
                PositionY -= m_LineSpacing;
                continue;
            }


            f64 TexCoordLeft, TexCoordBottom, TexCoordRight, TexCoordTop;
            Glyph->getQuadAtlasBounds(TexCoordLeft, TexCoordBottom, TexCoordRight, TexCoordTop);

            f64 PosLeft, PosBottom, PosRight, PosTop;
            Glyph->getQuadPlaneBounds(PosLeft, PosBottom, PosRight, PosTop);

            TexCoordLeft *= 1.0f / m_Font->GetAtlasTexture()->GetWidth();
            TexCoordRight *= 1.0f / m_Font->GetAtlasTexture()->GetWidth();
            TexCoordBottom *= 1.0f / m_Font->GetAtlasTexture()->GetHeight();
            TexCoordTop *= 1.0f / m_Font->GetAtlasTexture()->GetHeight();

            OutVertices.AddRange({
                Vertex(Vector3(PositionX + PosLeft, PositionY + PosBottom, 0.0f), Vector2(TexCoordLeft, TexCoordBottom), Vector3::Zero, Color::White),
                Vertex(Vector3(PositionX + PosLeft, PositionY + PosTop, 0.0f), Vector2(TexCoordLeft, TexCoordTop), Vector3::Zero, Color::White),
                Vertex(Vector3(PositionX + PosRight, PositionY + PosTop, 0.0f), Vector2(TexCoordRight, TexCoordTop), Vector3::Zero, Color::White),
                Vertex(Vector3(PositionX + PosRight, PositionY + PosBottom, 0.0f), Vector2(TexCoordRight, TexCoordBottom), Vector3::Zero, Color::White)
            });

            OutIndices.AddRange({
                0 + 4 * (u32)Index,
                2 + 4 * (u32)Index,
                1 + 4 * (u32)Index,
                0 + 4 * (u32)Index,
                3 + 4 * (u32)Index,
                2 + 4 * (u32)Index
            });

            if (Index != m_Text.Count() - 1)
            {
                const String::CharacterType NextCharacter = m_Text[Index + 1];
                f64 Advance = 0.0;
                FontGeometry.getAdvance(Advance, Character, NextCharacter);
                PositionX += Advance + m_CharacterSpacing;
            }
        }
    }
}
