#pragma once
#include "Runtime/Component.h"
#include "Runtime/Sprite.h"
#include "Math/Vector2.h"
#include "Rendering/Buffer.h"
#include "Rendering/GraphicsPipeline.h"
#include "Rendering/Sampler.h"
#include "Rendering/ShaderBindingSet.h"
#include "Rendering/SpriteRendererFlags.h"
#include "Runtime/Ref.h"

typedef struct VkPipelineLayout_T* VkPipelineLayout;
typedef struct VkDescriptorSet_T* VkDescriptorSet;
typedef struct VkDescriptorSetLayout_T* VkDescriptorSetLayout;

namespace Nova
{
    namespace Rendering { class Shader; }

    class SpriteAnimation;

    class SpriteRenderer final : public Component
    {
    public:
        SpriteRenderer(Entity* owner);

        void OnInit() override;
        void OnUpdate(float deltaTime) override;
        void OnPreRender(Rendering::CommandBuffer& cmdBuffer) override;
        void OnRender(Rendering::CommandBuffer& cmdBuffer) override;
        void OnDestroy() override;

        Sprite& GetSprite();
        void SetSprite(const Sprite& sprite);

        void SetSpriteAnimation(SpriteAnimation* spriteAnimation);
        SpriteAnimation* GetSpriteAnimation() const;

        Vector2 GetTiling() const;
        void SetTiling(const Vector2& tiling);

        uint32_t GetPixelsPerUnit() const;
        void SetPixelsPerUnit(uint32_t pixelsPerUnit);

        Color GetColorTint() const;
        void SetColorTint(const Color& color);

        SpriteRendererFlags GetFlags() const;
        void SetFlags(SpriteRendererFlags flags);

        void SetSpeed(int32_t fps);
        void SetSpeed(float speed);
        float GetSpeed() const;
    private:
        Sprite m_Sprite = { 0, 0, 0, 0, nullptr };
        SpriteAnimation* m_SpriteAnimation = nullptr;
        SpriteAnimation* m_PendingSpriteAnimation = nullptr;
        SpriteRendererFlags m_Flags = SpriteRendererFlagBits::None;
        uint32_t m_PixelsPerUnit = 128;
        Vector2 m_Tiling = Vector2::One;
        Color m_ColorTint = Color::White;

        float m_Time = 0.0f;
        float m_Speed = 1.0f / 20.0f;
        uint32_t m_SpriteIndex = 0;

        Ref<Rendering::Shader> m_Shader = nullptr;
        Ref<Rendering::ShaderBindingSet> m_BindingSet = nullptr;
        Ref<Rendering::Buffer> m_VertexBuffer = nullptr;
        Ref<Rendering::Buffer> m_IndexBuffer = nullptr;
        Ref<Rendering::Buffer> m_UniformBuffer = nullptr;
        Ref<Rendering::Buffer> m_StagingBuffer = nullptr;
        Ref<Rendering::Sampler> m_Sampler = nullptr;
        Ref<Rendering::GraphicsPipeline> m_Pipeline = nullptr;
    };


}
