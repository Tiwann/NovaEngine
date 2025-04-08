#include "Runtime/Entity.h"
#include "SpriteAnimator.h"
#include "SpriteRenderer.h"
#include "Runtime/SpriteAnimation.h"
#include "Editor/EditorGUI.h"

namespace Nova
{
    SpriteAnimator::SpriteAnimator(Entity* Owner) : Component(Owner, "Sprite Animator")
    {
        
    }

    void SpriteAnimator::OnInit()
    {
        Component::OnInit();
        m_SpriteRenderer = m_Entity->GetComponent<SpriteRenderer>();
        
        m_Timer.FinishedEvent.BindMember(this, &SpriteAnimator::OnSpriteChange);
        m_Timer.SetDuration(1.0f / (f32)m_Speed);
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

    void SpriteAnimator::OnUpdate(f32 Delta)
    {
        Component::OnUpdate(Delta);
        if(!m_SpriteRenderer) return;
        if(!m_Animation) return;
        
        m_Timer.SetDuration(1.0f / (f32)m_Speed);
        m_Timer.Update(Delta);
    }

    void SpriteAnimator::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);
        UI::DragValue<i32>("Speed (FPS)", m_Speed, 1, 60);
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

    void SpriteAnimator::SetSpeed(i32 Speed)
    {
        m_Speed = Math::Max(0, Speed);
    }

    i32 SpriteAnimator::GetSpeed() const
    {
        return m_Speed;
    }

    void SpriteAnimator::OnSpriteChange()
    {
        m_Index = (m_Index + 1) % m_Animation->Count();
        const Sprite& NewSprite = m_Animation->GetSprite(m_Index);
        m_SpriteRenderer->SetSprite(NewSprite);
        
        const Array<AnimationNotify>& NotifiesView = m_Animation->GetNotifies();
        Array<AnimationNotify*> FiringNotifies = NotifiesView.Where([this](const AnimationNotify& Notify)
        {
            return Notify.Frame == m_Index;
        });
        
        for(AnimationNotify* Notify : FiringNotifies)
        {
            if(Notify->Event.IsBound())
            {
                Notify->Event.Broadcast();
            }
        }
    }
}
