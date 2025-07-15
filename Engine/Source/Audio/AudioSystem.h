#pragma once

#include <miniaudio.h>
#include <cstdint>


namespace Nova
{
    class AudioClip;

    class AudioSystem
    {
    public:
        AudioSystem();
        bool Initialize(uint32_t Channels, uint32_t SampleRate, uint32_t ListenerCount);
        void Shutdown();

        AudioClip* CreateClipFromFile(const char* filepath);
        AudioClip* CreateClipFromMemory(const uint8_t* data, size_t size);
        void PlayAudioClip(AudioClip* clip);
        void StopAudioClip(AudioClip* clip);
        void PauseAudioClip(AudioClip* clip);

        const ma_engine* GetHandle() const { return &m_Engine; }
        ma_engine* GetHandle() { return &m_Engine; }
    private:
        ma_engine m_Engine;
    };
}
