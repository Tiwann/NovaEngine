#pragma once
#include "SpriteRenderer.h"
#include "../../Runtime/Component.h"
#include "Runtime/Timer.h"

namespace Nova
{
    class SpriteAnimation;
    
    class SpriteAnimator : public Component
    {
    public:
        SpriteAnimator(Entity* owner);
        
        void OnInit() override;
        void OnStart() override;
        void OnUpdate(float Delta) override;

        SpriteRenderer* GetSpriteRenderer() const;
        void SetSpriteRenderer(SpriteRenderer* SpriteRenderer);

        const SpriteAnimation* GetAnimation() const;
        void SetAnimation(SpriteAnimation* Animation);

        void SetSpeed(int32_t speed);
        int32_t GetSpeed() const;
    private:
        Timer m_Timer;
        SpriteRenderer* m_SpriteRenderer = nullptr;
        SpriteAnimation* m_Animation = nullptr;
        int32_t m_Speed = 12;
        uint32_t m_Index = 0;
        void OnSpriteChange();
    };
}
