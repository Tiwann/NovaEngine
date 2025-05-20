#include "SpriteRenderer.h"
#include "Components/Transform.h"
#include "Containers/StaticArray.h"
#include "Runtime/Application.h"
#include "Runtime/Scene.h"
#include "Runtime/Entity.h"
#include "Runtime/Color.h"
#include "Rendering/VertexArray.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/VertexLayout.h"
#include "Rendering/Shader.h"
#include "Rendering/Renderer.h"
#include "Rendering/Texture2D.h"
#include "ResourceManager/ShaderManager.h"
#include "Editor/EditorGUI.h"

namespace Nova
{
    SpriteRenderer::SpriteRenderer(Entity* Owner) : Component(Owner, "Sprite Renderer"),
    m_Sprite(nullptr, Vector2::Zero, Vector2::Zero), m_Tiling(Vector2::One)
    {
    }

    void SpriteRenderer::OnInit()
    {
        Component::OnInit();
        const Entity* Owner = GetOwner();
        const Scene* CurrentScene = Owner->GetScene();
        const Application* CurrentApplication = CurrentScene->GetOwner();
        Renderer* Renderer = CurrentApplication->GetRenderer();

        m_VertexArray = VertexArray::Create(Renderer->GetGraphicsApi());
        m_VertexArray->Bind();

        constexpr u32 Indices[6] { 0, 2, 1, 0, 3, 2 };
        m_VertexBuffer = Renderer->CreateVertexBuffer(BufferView<Vertex>(nullptr, 0));
        m_IndexBuffer = Renderer->CreateIndexBuffer(BufferView<u32>{ Indices, 6 });

        ShaderManager* Manager = g_Application->GetShaderManager();
        m_Shader = Manager->Retrieve("Sprite");
    }

    void SpriteRenderer::OnDestroy()
    {
        Component::OnDestroy();
        delete m_IndexBuffer;
        delete m_VertexBuffer;
        delete m_VertexArray;
        
        m_IndexBuffer = nullptr;
        m_VertexBuffer = nullptr;
        m_VertexArray = nullptr;
    }

    
    void SpriteRenderer::OnRender(Renderer* Renderer)
    {
        Component::OnRender(Renderer);
        if(!m_Shader) return;
        if(!m_Sprite.GetTexture()) return;

        const Vector2 Position = m_Sprite.GetPosition();
        const Vector2 Size = m_Sprite.GetSize();
        const f32 Width = m_Sprite.GetTexture()->GetSize().x;
        const f32 Height = m_Sprite.GetTexture()->GetSize().y;
        
        Vector2 Uv0 = {Position.x / Width, (Position.y + Size.y) / Height};
        Vector2 Uv1 = {(Position.x + Size.x) / Width, (Position.y + Size.y) / Height};
        Vector2 Uv2 = {(Position.x + Size.x) / Width, Position.y / Height};
        Vector2 Uv3 = {Position.x / Width, Position.y / Height};

        if(Flags.Contains(SpriteRendererFlagBits::FlipHorizontal))
        {
            std::swap(Uv0, Uv1);
            std::swap(Uv2, Uv3);
        }

        if(Flags.Contains(SpriteRendererFlagBits::FlipVertical))
        {
            std::swap(Uv0, Uv2);
            std::swap(Uv1, Uv3);
        }
         
        StaticArray<Vertex, 4> SpriteVertices = {
            {{-0.5f, +0.5f, 0.0f}, Uv0, Vector3::Zero, Color::White},
            {{+0.5f, +0.5f, 0.0f}, Uv1, Vector3::Zero, Color::White},
            {{+0.5f, -0.5f, 0.0f}, Uv2, Vector3::Zero, Color::White},
            {{-0.5f, -0.5f, 0.0f}, Uv3, Vector3::Zero, Color::White},
        };
        
        m_VertexArray->Bind();
        m_VertexBuffer->Initialize(VertexBufferCreateInfo(SpriteVertices));

        m_Shader->Bind();
        m_Shader->SetUniformMat4("uModel", GetTransform()->GetWorldSpaceMatrix());

        if(m_Sprite.GetTexture())
        {
            m_Shader->SetUniformTexture("uTexture", m_Sprite.GetTexture());
        }
        
        
        const Vector2 Tiling = Flags.Contains(SpriteRendererFlagBits::TileWithScale) ? (Vector2)GetTransform()->GetScale() : m_Tiling;
        m_Shader->SetUniformFloat2("uTiling", Tiling);
        m_Shader->SetUniformFloat4("uColorTint", m_ColorTint);
        const Matrix2 SpriteScale = m_Sprite.GetTexture() ? Math::Scale(Matrix2::Identity, m_Sprite.GetSize() / (f32)m_PixelsPerUnit) : Matrix2::Identity;
        m_Shader->SetUniformMat2("uSpriteScale", SpriteScale);
        //Renderer->DrawIndexed(TODO);
    }

    void SpriteRenderer::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);

        
        UI::Sprite(m_Sprite);

        UI::DragVector2<f32>("Tiling", m_Tiling, 0.1f);
        UI::DragVector2<f32>("Size", m_Size, 0.1f);
        
        const char* FlagNames[] = { "None",
                                    "Tile With Scale",
                                    "Flip Horizontal",
                                    "Flip Vertical",
                                    "Normalize Size" };

        
        if(ImGui::BeginCombo("Flags", nullptr, ImGuiComboFlags_NoPreview))
        {
            for(size_t i = 1; i < std::size(FlagNames); i++)
            {
                bool IsSelected = Flags.Contains(SpriteRendererFlags(1 << i));
                if(ImGui::Selectable(FlagNames[i], &IsSelected))
                {
                    Flags.Toggle(SpriteRendererFlags(1 << i));
                }
            }
            ImGui::EndCombo();
        }

        ImGui::ColorEdit4("Tint", (f32*)&m_ColorTint, ImGuiColorEditFlags_DisplayHex);
        UI::DragValue<int>("Pixels Per Unit", m_PixelsPerUnit, 1);
    }

    void SpriteRenderer::SetTiling(const Vector2& Tiling)
    {
        m_Tiling = Tiling;
    }

    Vector2 SpriteRenderer::GetTiling() const
    {
        return m_Tiling;
    }

    void SpriteRenderer::SetSize(const Vector2& Size)
    {
        m_Size = Size;
    }

    Vector2 SpriteRenderer::GetSize() const
    {
        return m_Size;
    }

    Color SpriteRenderer::GetColorTint() const
    {
        return m_ColorTint;
    }

    void SpriteRenderer::SetColorTint(const Color& Color)
    {
        m_ColorTint = Color;
    }

    i32 SpriteRenderer::GetPixelsPerUnit() const
    {
        return m_PixelsPerUnit;
    }

    void SpriteRenderer::SetPixelsPerUnit(i32 PixelsPerUnit)
    {
        m_PixelsPerUnit = PixelsPerUnit;
    }

    Sprite SpriteRenderer::GetSprite() const
    {
        return m_Sprite;
    }

    void SpriteRenderer::SetSprite(const Sprite& sprite)
    {
        m_Sprite = sprite;
    }
}
