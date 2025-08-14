#include "../../Runtime/SpriteAnimation.h"
#include "Runtime/Entity.h"
#include "SpriteAnimator.h"
#include "SpriteRenderer.h"

#include <print>

namespace Nova
{
    SpriteAnimator::SpriteAnimator(Entity* owner) : Component(owner, "Sprite Animator")
    {
        
    }

    void SpriteAnimator::OnInit()
    {
        Component::OnInit();
        m_SpriteRenderer = m_Entity->GetComponent<SpriteRenderer>();
    }

    void SpriteAnimator::OnStart()
    {
        Component::OnStart();
        m_Index = 0;
        const Sprite& Sprite = m_Animation->GetSprite(m_Index);
        m_SpriteRenderer->SetSprite(Sprite);
    }

    void SpriteAnimator::OnUpdate(const float deltaTime)
    {
        Component::OnUpdate(deltaTime);
        if(!m_SpriteRenderer) return;
        if(!m_Animation) return;

        const float duration = 1.0f / m_Speed;
        m_Time += deltaTime;
        if (m_Time >= duration)
        {
            m_Time = 0.0f;
            m_Index = (m_Index + 1) % m_Animation->Count();
            const Sprite& newSprite = m_Animation->GetSprite(m_Index);
            m_SpriteRenderer->SetSprite(newSprite);
            std::println("{}", m_Index);
        }
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
        m_Time = 0.0f;
    }

    void SpriteAnimator::SetSpeed(const int32_t speed)
    {
        m_Speed = Math::Max(0, speed);
    }

    int32_t SpriteAnimator::GetSpeed() const
    {
        return m_Speed;
    }
}
