#include "TweenManager.h"

#include "Components/Camera.h"
#include "Math/Vector3.h"
#include "Components/Transform.h"
#include "Components/Audio/SoundEmitter.h"

namespace Nova
{
    TweenManager& TweenManager::Get()
    {
        static TweenManager Instance;
        return Instance;
    }

    void TweenManager::AddTween(Tweener* Tweener)
    {
        Get().m_Tweens.Add(Tweener);
    }

    void TweenManager::Update(f32 Delta)
    {
        TweenManager& Manager = TweenManager::Get();
        for (Tweener* Tween : Manager.m_Tweens)
        {
            if (Tween->IsFinished())
            {
                Manager.m_Tweens.Remove(Tween);
                delete Tween;
                continue;
            }
            Tween->Update(Delta);
        }
    }

    TweenerValue<Vector3>& TweenManager::DoPosition(Transform* Transform, Vector3 EndValue, f32 Duration)
    {
        TweenManager& Manager = TweenManager::Get();
        TweenerValue<Vector3>* NewTween = new TweenerValue<Vector3>(&Manager, [Transform]{return Transform->GetPosition(); }, [Transform](const Vector3& EndValue) {Transform->SetPosition(EndValue); });
        NewTween->WithDuration(Duration);
        NewTween->ChangeEndValue(EndValue);
        Manager.m_Tweens.Add(NewTween);
        return *NewTween;
    }

    TweenerValue<Vector3>& TweenManager::DoRotation(Transform* Transform, Vector3 EndValue, f32 Duration)
    {
        TweenManager& Manager = TweenManager::Get();
        TweenerValue<Vector3>* NewTween = new TweenerValue<Vector3>(&Manager, [Transform]{return Transform->GetRotation().ToEulerDegrees(); }, [Transform](const Vector3& EndValue) {Transform->SetRotation(Quaternion::FromEulerDegrees(EndValue)); });
        NewTween->WithDuration(Duration);
        NewTween->ChangeEndValue(EndValue);
        Manager.m_Tweens.Add(NewTween);
        return *NewTween;
    }

    TweenerValue<Vector3>& TweenManager::DoScale(Transform* Transform, Vector3 EndValue, f32 Duration)
    {
        TweenManager& Manager = TweenManager::Get();
        TweenerValue<Vector3>* NewTween = new TweenerValue<Vector3>(&Manager, [Transform]{return Transform->GetScale(); }, [Transform](const Vector3& EndValue) {Transform->SetScale(EndValue); });
        NewTween->WithDuration(Duration);
        NewTween->ChangeEndValue(EndValue);
        Manager.m_Tweens.Add(NewTween);
        return *NewTween;
    }

    TweenerRef<Color>& TweenManager::DoColor(Color& InOutColor, const Color& EndValue, f32 Duration)
    {
        TweenManager& Manager = TweenManager::Get();
        TweenerRef<Color>* NewTween = new TweenerRef<Color>(&Manager, InOutColor);
        NewTween->WithDuration(Duration);
        NewTween->ChangeEndValue(EndValue);
        Manager.m_Tweens.Add(NewTween);
        return *NewTween;
    }

    TweenerValue<f32>& TweenManager::DoFade(SoundEmitter* Emitter, f32 EndValue, f32 Duration)
    {
        TweenManager& Manager = TweenManager::Get();
        TweenerValue<f32>* NewTween = new TweenerValue<f32>(&Manager, [Emitter]{return Emitter->GetVolume(); }, [Emitter](const f32& Val) {Emitter->SetVolume(Val); });
        NewTween->WithDuration(Duration);
        NewTween->ChangeEndValue(EndValue);
        Manager.m_Tweens.Add(NewTween);
        return *NewTween;
    }

    TweenerValue<f32>& TweenManager::DoPitch(SoundEmitter* Emitter, f32 EndValue, f32 Duration)
    {
        TweenManager& Manager = TweenManager::Get();
        TweenerValue<f32>* NewTween = new TweenerValue<f32>(&Manager, [Emitter]{return Emitter->GetPitch(); }, [Emitter](const f32& Val) {Emitter->SetPitch(Val); });
        NewTween->WithDuration(Duration);
        NewTween->ChangeEndValue(EndValue);
        Manager.m_Tweens.Add(NewTween);
        return *NewTween;
    }

    TweenerRef<Color>& TweenManager::DoClearColor(Camera* Camera, Color EndValue, f32 Duration)
    {
        TweenManager& Manager = Get();
        TweenerRef<Color>* NewTween = new TweenerRef<Color>(&Manager, Camera->ClearColor);
        NewTween->WithDuration(Duration);
        NewTween->ChangeEndValue(EndValue);
        Manager.m_Tweens.Add(NewTween);
        return *NewTween;
    }
}
