#pragma once
#include "Runtime/Component.h"
#include "Runtime/LogCategory.h"
#include "Containers/MulticastDelegate.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(SoundEmitter, "SOUND EMITTER")

namespace FMOD
{
    class System;
    class Channel;
}

namespace Nova
{
    class Sound;

    class SoundEmitter : public Component
    {
    public:
        using EmitterStartedDelegate = MulticastDelegate<void(Sound* Sound, bool WasPaused)>;
        using EmitterStopDelegate = MulticastDelegate<void(Sound* Sound, bool IsPause)>;
        using EmitterPlayingDelegate = MulticastDelegate<void(Sound* Sound, u32 Position, u32 Length)>;
        
        SoundEmitter(Entity* Owner);

        void OnInit() override;
        void OnUpdate(f32 Delta) override;
        void OnInspectorGUI(const ImGuiIO& IO) override;
        void OnDestroy() override;

        void Play();
        void Stop();
        bool IsPlaying() const;
        void Pause();
        void Resume();

        Sound* GetSound() const;
        void SetSound(Sound* Clip);
        void SetVolume(f32 Vol);
        void SetPitch(f32 Pitch);
        f32 GetVolume() const;
        f32 GetPitch() const;

        EmitterStartedDelegate OnStartedEvent;
        EmitterStopDelegate OnStopEvent;
        EmitterPlayingDelegate OnPlayingEvent;
    
        void OnPlaying(Sound* Sound, u32 Position, u32 Length);
    private:
        Sound* m_Sound = nullptr;
        f32 m_Volume = 1.0f;
        f32 m_Pitch = 1.0f;
        bool m_Looping = false;
        u32 m_PlaybackPosition = 0;
        u32 m_Length = 0;
        
        FMOD::Channel* m_Channel = nullptr;
    };
}
