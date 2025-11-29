#pragma once
#include "Runtime/Component.h"
#include "Containers/MulticastDelegate.h"
#include "Math/Vector3.h"
#include "Runtime/Ref.h"
#include <cstdint>

namespace Nova
{
    class AudioClip;

    enum class PlayMode
    {
        Mode2D,
        Mode3D
    };

    class AudioSource final : public Component
    {
    public:
        using StartedDelegate = MulticastDelegate<void(Ref<AudioClip> audioClip, bool wasPaused)>;
        using StoppedDelegate = MulticastDelegate<void(Ref<AudioClip> audioClip, bool isPause)>;
        using PlayingDelegate = MulticastDelegate<void(Ref<AudioClip> audioClip, uint32_t position, uint32_t length)>;
        
        explicit AudioSource(Entity* owner);

        void OnUpdate(float deltaTime) override;
        //void OnInspectorGUI(const ImGuiIO& IO) override;
        void OnDestroy() override;

        void Play();
        void Stop();
        bool IsPlaying() const;
        bool IsPaused() const;
        bool IsLooping() const;
        void Pause();
        void Resume();

        Ref<AudioClip> GetAudioClip() const;
        void SetAudioClip(Ref<AudioClip> clip);
        void SetVolume(float volume);
        void SetPitch(float pitch);
        void SetPan(float pan);
        void SetIsLooping(bool looping);
        void SetIsSpatialized(bool spatialized);
        float GetVolume() const;
        float GetPitch() const;

        StartedDelegate OnStartedEvent;
        StoppedDelegate OnStoppedEvent;
        PlayingDelegate OnPlayingEvent;

        BufferView<float> GetFrequencies() const;
    private:
        Ref<AudioClip> m_Clip = nullptr;
        float m_Volume = 1.0f;
        float m_Pitch = 1.0f;
        float m_Pan = 0.0f;
        bool m_Looping = false;
        bool m_Paused = false;
        bool m_Spatialized = false;
        uint64_t m_PositionFrames = 0;
        uint64_t m_DurationFrames = 0;
        Vector3 m_LastPosition = Vector3::Zero;
    };
}
