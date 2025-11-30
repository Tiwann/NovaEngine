#include "AudioSource.h"
#include "Audio/AudioClip.h"
#include "Components/Transform.h"
#include "Audio/AudioSystem.h"
#include "Containers/String.h"
#include "Containers/StringFormat.h"
#include <imgui.h>

namespace Nova
{

    AudioSource::AudioSource(Entity* owner) : Component(owner, "Audio Source")
    {

    }

    void AudioSource::OnDestroy()
    {
        Component::OnDestroy();
        Stop();
        OnStartedEvent.ClearAll();
        OnStoppedEvent.ClearAll();
        OnPlayingEvent.ClearAll();
    }

    void AudioSource::OnUpdate(const float deltaTime)
    {
        Component::OnUpdate(deltaTime);

        if (!m_Clip) return;

        ma_sound_set_volume(m_Clip->GetHandle(), m_Volume);
        ma_sound_set_pitch(m_Clip->GetHandle(), m_Pitch);
        ma_sound_set_pan(m_Clip->GetHandle(), m_Pan);
        ma_sound_set_looping(m_Clip->GetHandle(), m_Looping);
        ma_sound_set_spatialization_enabled(m_Clip->GetHandle(), m_Spatialized);

        m_PositionFrames = ma_sound_get_time_in_pcm_frames(m_Clip->GetHandle());
        ma_sound_get_length_in_pcm_frames(m_Clip->GetHandle(), &m_DurationFrames);
        if(IsPlaying())
            OnPlayingEvent.Broadcast(m_Clip, m_PositionFrames, m_DurationFrames);

        const Transform* transform = GetTransform();
        const Vector3 position = transform->GetPosition();
        const Vector3 forward = transform->GetForwardVector();
        const Vector3 velocity = (position - m_LastPosition) / deltaTime;
        m_LastPosition = position;
        ma_sound_set_position(m_Clip->GetHandle(), position.x, position.y, position.z);
        ma_sound_set_velocity(m_Clip->GetHandle(), velocity.x, velocity.y, velocity.z);
        ma_sound_set_direction(m_Clip->GetHandle(), forward.x, forward.y, forward.z);
        ma_sound_set_cone(m_Clip->GetHandle(), Math::Tau, Math::Tau, 1.0f);
    }

    void AudioSource::OnGui()
    {
        Component::OnGui();

        if (ImGui::TreeNode("Audio Clip"))
        {
            const String uuid = m_Clip ? m_Clip->GetUuid().GetString() : "None";
            const String text = StringFormat("UUID: {}", uuid);
            ImGui::TextUnformatted(StringView(text));

            ImGui::Separator();
            const bool playing = IsPlaying();
            const char* label = IsPlaying() ? "Stop" : "Play";
            if (ImGui::Button(label))
            {
                if (!playing) Play(); else; Stop();
            }
            ImGui::TreePop();
        }

        ImGui::DragFloat("Volume", &m_Volume, 0.01f, 0.0f, 1.0f, "%.2f");
        ImGui::DragFloat("Pitch", &m_Pitch, 0.01f, 0.0f, 0.0f, "%.2f");
        ImGui::DragFloat("Pan", &m_Pan, 0.01f, -1.0f, 1.0f, "%.2f");
        ImGui::Checkbox("Looping", &m_Looping);
        ImGui::Checkbox("Spatialized", &m_Spatialized);
    }



    void AudioSource::Play()
    {
        AudioSystem* audioSystem = AudioSystem::GetInstance();
        audioSystem->PlayAudioClip(m_Clip);
        OnStartedEvent.Broadcast(m_Clip, false);
    }

    void AudioSource::Stop()
    {
        if (!m_Clip) return;
        AudioSystem* audioSystem = AudioSystem::GetInstance();
        audioSystem->StopAudioClip(m_Clip);
        ma_sound_set_start_time_in_pcm_frames(m_Clip->GetHandle(), 0);
        OnStoppedEvent.Broadcast(m_Clip, false);
    }

    void AudioSource::Pause()
    {
        AudioSystem* audioSystem = AudioSystem::GetInstance();
        audioSystem->StopAudioClip(m_Clip);
        OnStoppedEvent.Broadcast(m_Clip, true);
        m_Paused = true;
    }

    void AudioSource::Resume()
    {
        if (m_Paused)
        {
            AudioSystem* audioSystem = AudioSystem::GetInstance();
            audioSystem->PlayAudioClip(m_Clip);
            OnStartedEvent.Broadcast(m_Clip, false);
            m_Paused = false;
        }
    }

    Ref<AudioClip> AudioSource::GetAudioClip() const
    {
        return m_Clip;
    }

    void AudioSource::SetAudioClip(Ref<AudioClip> clip)
    {
        if(IsPlaying()) Stop();
        m_Clip = clip;
    }

    void AudioSource::SetVolume(const float volume)
    {
        m_Volume = volume;
    }

    void AudioSource::SetPitch(const float pitch)
    {
        m_Pitch = pitch;
    }

    void AudioSource::SetPan(const float pan)
    {
        m_Pan = pan;
    }

    void AudioSource::SetIsLooping(bool looping)
    {
        m_Looping = looping;
    }

    void AudioSource::SetIsSpatialized(bool spatialized)
    {
        m_Spatialized = spatialized;
    }

    float AudioSource::GetVolume() const
    {
        return m_Volume;
    }

    float AudioSource::GetPitch() const
    {
        return m_Pitch;
    }

    BufferView<float> AudioSource::GetFrequencies() const
    {
        if (!IsPlaying()) return { nullptr, 0 };

        Ref<FFTAudioNode> fftNode = m_Clip->GetFFTAudioNode();
        if (!fftNode) return { nullptr, 0 };

        return fftNode->GetFrequencies();
    }

    bool AudioSource::IsPlaying() const
    {
        return m_Clip ? ma_sound_is_playing(m_Clip->GetHandle()) : false;
    }

    bool AudioSource::IsPaused() const
    {
        return m_Paused;
    }

    bool AudioSource::IsLooping() const
    {
        return ma_sound_is_looping(m_Clip->GetHandle());
    }
}
