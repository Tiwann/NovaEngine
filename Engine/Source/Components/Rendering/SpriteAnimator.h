#pragma once
#include "SpriteRenderer.h"
#include "Runtime/Component.h"

namespace Nova
{
    class SpriteAnimation;
    
    class SpriteAnimator final : public Component
    {
    public:
        SpriteAnimator(Entity* owner);
        
        void OnInit() override;
        void OnStart() override;
        void OnUpdate(float deltaTime) override;

        SpriteRenderer* GetSpriteRenderer() const;
        void SetSpriteRenderer(SpriteRenderer* SpriteRenderer);

        const SpriteAnimation* GetAnimation() const;
        void SetAnimation(SpriteAnimation* Animation);

        void SetSpeed(int32_t speed);
        int32_t GetSpeed() const;
    private:
        float m_Time = 0.0f;
        SpriteRenderer* m_SpriteRenderer = nullptr;
        SpriteAnimation* m_Animation = nullptr;
        int32_t m_Speed = 12;
        uint32_t m_Index = 0;
    };
}
