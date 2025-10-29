#pragma once
#include "Containers/StringView.h"
#include <miniaudio.h>
#include <cstdint>

#include "Runtime/Ref.h"


namespace Nova
{
    class AudioClip;

    struct AudioSystemCreateInfo
    {
        uint32_t channels = 2;
        uint32_t sampleRate = 44100;
        uint32_t listenerCount = 1;
    };

    class AudioSystem
    {
    public:
        AudioSystem();
        [[deprecated("Deprecated use the version that accepts an AudioSystemCreateInfo structure instead")]]
        bool Initialize(uint32_t channels, uint32_t sampleRate, uint32_t listenerCount);
        bool Initialize(AudioSystemCreateInfo createInfo);
        void Destroy();

        AudioClip* CreateClipFromFile(StringView filepath);
        AudioClip* CreateClipFromMemory(const uint8_t* data, size_t size);

        void PlayAudioClip(AudioClip* clip);
        void StopAudioClip(AudioClip* clip);

        const ma_engine* GetHandle() const { return &m_Engine; }
        ma_engine* GetHandle() { return &m_Engine; }

        static AudioSystem* GetInstance();
    private:
        ma_engine m_Engine;
        static AudioSystem* s_Instance;
    };

    Ref<AudioSystem> CreateAudioSystem(const AudioSystemCreateInfo& createInfo);
}
