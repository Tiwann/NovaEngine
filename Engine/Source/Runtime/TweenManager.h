#pragma once
#include "Containers/DynamicArray.h"
#include "Tweener.h"
#include "Graphics/Texture2D.h"

namespace Nova
{
    class Transform;
    class SoundEmitter;
    class Camera;
    
    class TweenManager
    {
    public:
        TweenManager(const TweenManager&) = delete;
        TweenManager(TweenManager&&) = delete;
        TweenManager& operator=(const TweenManager&) = delete;
        TweenManager& operator=(TweenManager&&) = delete;
        
        static TweenManager& Get();
        static void AddTween(Tweener* Tweener);
        static void Update(f32 Delta);


        template<typename T>
        static TweenerValue<T>& DoValue(const typename TweenerValue<T>::Getter& Getter, const typename TweenerValue<T>::Setter& Setter, T EndValue, f32 Duration, TweenEase Ease = TweenEase::Linear)
        {
            TweenManager& Manager = TweenManager::Get();
            TweenerValue<T>* NewTween = new TweenerValue<T>(&Manager, Getter, Setter);
            NewTween->WithDuration(Duration);
            NewTween->ChangeEndValue(EndValue);
            Manager.m_Tweens.Add(dynamic_cast<Tweener*>(NewTween));
            return *NewTween;
        }
        
        
        template<typename T>
        static TweenerRef<T>& DoValue(T& Ref,T EndValue, f32 Duration)
        {
            TweenManager& Manager = TweenManager::Get();
            TweenerRef<T>* NewTween = new TweenerRef<T>(&Manager, Ref);
            NewTween->WithDuration(Duration);
            NewTween->ChangeEndValue(EndValue);
            Manager.m_Tweens.Add(dynamic_cast<Tweener*>(NewTween));
            return *NewTween;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////
        ////// TRANSFORM API
        /////////////////////////////////////////////////////////////////////////////////////////////////
        static TweenerValue<Vector3>& DoPosition(Transform* Transform, Vector3 EndValue, f32 Duration);
        static TweenerValue<Vector3>& DoRotation(Transform* Transform, Vector3 EndValue, f32 Duration);
        static TweenerValue<Vector3>& DoScale(Transform* Transform, Vector3 EndValue, f32 Duration);
        
        static TweenerRef<Color>&     DoColor(Color& InOutColor, const Color& EndValue, f32 Duration);


        /////////////////////////////////////////////////////////////////////////////////////////////////
        ////// SOUND EMITTER API
        /////////////////////////////////////////////////////////////////////////////////////////////////
        static TweenerValue<f32>&     DoFade(SoundEmitter* Emitter, f32 EndValue, f32 Duration);
        static TweenerValue<f32>&     DoPitch(SoundEmitter* Emitter, f32 EndValue, f32 Duration);

        /////////////////////////////////////////////////////////////////////////////////////////////////
        ////// CAMERA
        /////////////////////////////////////////////////////////////////////////////////////////////////
        static TweenerRef<Color>&     DoClearColor(Camera* Camera, Color EndValue, f32 Duration);
    private:
        TweenManager() = default;
        Array<Tweener*> m_Tweens;
    };
}
