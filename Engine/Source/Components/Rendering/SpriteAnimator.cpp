#include "Assets/SpriteAnimation.h"
#include "../../Runtime/Entity.h"
#include "SpriteAnimator.h"
#include "SpriteRenderer.h"

namespace Nova
{
    SpriteAnimator::SpriteAnimator(Entity* owner) : Component(owner, "Sprite Animator")
    {
        
    }

    void SpriteAnimator::OnInit()
    {
        Component::OnInit();
        m_SpriteRenderer = m_Entity->GetComponent<SpriteRenderer>();
        m_Timer.finishedEvent.BindMember(this, &SpriteAnimator::OnSpriteChange);

        m_Timer.SetDuration(1.0f / (float)m_Speed);
        m_Timer.SetLoop(true);
        m_Timer.Start();
    }

    void SpriteAnimator::OnStart()
    {
        Component::OnStart();
        m_Index = 0;
        const Sprite& Sprite = m_Animation->GetSprite(m_Index);
        m_SpriteRenderer->SetSprite(Sprite);
    }

    void SpriteAnimator::OnUpdate(float Delta)
    {
        Component::OnUpdate(Delta);
        if(!m_SpriteRenderer) return;
        if(!m_Animation) return;
        
        m_Timer.SetDuration(1.0f / (float)m_Speed);
        m_Timer.Update(Delta);
    }

    SpriteRenderer* SpriteAnimator::GetSpriteRenderer() const
    {
        return m_SpriteRenderer;
    }

    void SpriteAnimator::SetSpriteRenderer(SpriteRenderer* SpriteRenderer)
    {
        m_SpriteRenderer = SpriteRenderer;
    }

    const SpriteAnimation* SpriteAnimator::GetAnimation() const
    {
        return m_Animation;
    }

    void SpriteAnimator::SetAnimation(SpriteAnimation* Animation)
    {
        m_Animation = Animation;
        m_Index = 0;
        m_Timer.Reset();
    }

    void SpriteAnimator::SetSpeed(const int32_t speed)
    {
        m_Speed = Math::Max(0, speed);
    }

    int32_t SpriteAnimator::GetSpeed() const
    {
        return m_Speed;
    }

    void SpriteAnimator::OnSpriteChange()
    {
        m_Index = (m_Index + 1) % m_Animation->Count();
        const Sprite& NewSprite = m_Animation->GetSprite(m_Index);
        m_SpriteRenderer->SetSprite(NewSprite);
    }
}
