#pragma once
#include "AudioFormat.h"
#include "Runtime/Object.h"
#include "Runtime/Ref.h"
#include "Containers/StringView.h"
#include <cstdint>
#include <miniaudio.h>


namespace Nova
{
    class AudioClip;
    class AudioNode;
    struct AudioNodeCreateInfo;

    struct AudioSystemCreateInfo
    {
        uint32_t channels = 2;
        uint32_t sampleRate = 44100;
        uint32_t listenerCount = 1;
    };

    class AudioSystem final : public Object
    {
    public:
        AudioSystem();
        bool Initialize(const AudioSystemCreateInfo& createInfo);
        void Destroy();

        void PlayAudioClip(AudioClip* clip);
        void StopAudioClip(AudioClip* clip);

        const ma_engine* GetHandle() const { return &m_Engine; }
        ma_engine* GetHandle() { return &m_Engine; }

        static AudioSystem* GetInstance();
        static ma_engine* GetInstanceHandle();

        uint32_t GetOutputChannelCount() const;
        uint32_t GetOutputSampleRate() const;
        AudioFormat GetOutputFormat() const;

        bool AttachAudioNodeToOutputBus(Ref<AudioNode> audioNode);
        void DetachAudioNodeFromOutputBus(const Ref<AudioNode>& audioNode);
    private:
        ma_engine m_Engine;
        static AudioSystem* s_Instance;
        uint32_t m_Channels = 0;
        uint32_t m_SampleRate = 0;
        Array<Ref<AudioNode>> m_AudioNodes;
    };

    Ref<AudioSystem> CreateAudioSystem(const AudioSystemCreateInfo& createInfo);
}
