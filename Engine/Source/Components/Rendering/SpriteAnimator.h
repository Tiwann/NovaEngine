#pragma once
#include "SpriteRenderer.h"
#include "Runtime/Component.h"
#include "Runtime/Timer.h"

namespace Nova
{
    class SpriteAnimation;
    
    class SpriteAnimator : public Component
    {
    public:
        SpriteAnimator(Entity* Owner);
        
        void OnInit() override;
        void OnStart() override;
        void OnUpdate(f32 Delta) override;
        void OnInspectorGUI(const ImGuiIO& IO) override;

        SpriteRenderer* GetSpriteRenderer() const;
        void SetSpriteRenderer(SpriteRenderer* SpriteRenderer);

        const SpriteAnimation* GetAnimation() const;
        void SetAnimation(SpriteAnimation* Animation);

        void SetSpeed(i32 Speed);
        i32 GetSpeed() const;
    private:
        Timer m_Timer;
        SpriteRenderer* m_SpriteRenderer;
        SpriteAnimation* m_Animation;
        i32 m_Speed = 12;
        u32 m_Index = 0;
        void OnSpriteChange();
    };
}
