#pragma once
#include "AudioFormat.h"
#include "Runtime/RefCounted.h"
#include "Runtime/Ref.h"
#include "Containers/StringView.h"
#include <cstdint>
#include <miniaudio.h>


namespace Nova
{
    class AudioClip;
    class AudioNode;
    struct AudioNodeCreateInfo;

    struct AudioDeviceCreateInfo
    {
        uint32_t channels = 2;
        uint32_t sampleRate = 44100;
        uint32_t listenerCount = 1;
    };

    class AudioDevice final : public RefCounted
    {
    public:
        AudioDevice();
        bool Initialize(const AudioDeviceCreateInfo& createInfo);
        void Destroy();

        void PlayAudioClip(AudioClip* clip);
        void StopAudioClip(AudioClip* clip);

        const ma_engine* GetHandle() const { return &m_Engine; }
        ma_engine* GetHandle() { return &m_Engine; }

        static AudioDevice* GetInstance();
        static ma_engine* GetInstanceHandle();

        uint32_t GetOutputChannelCount() const;
        uint32_t GetOutputSampleRate() const;
        AudioFormat GetOutputFormat() const;

        bool AttachAudioNodeToOutputBus(Ref<AudioNode> audioNode);
        void DetachAudioNodeFromOutputBus(const Ref<AudioNode>& audioNode);

    protected:
        static void* OnMalloc( size_t size, void* userData);
        static void* OnRealloc(void* where, size_t size, void* userData);
        static void OnFree(void* ptr, void* userData);
        static void AudioDataProc(ma_device* device, void* output, const void* input, ma_uint32 frameCount);
        static void OnNotification(const ma_device_notification* notification);
        static void OnProcess(void* userData, float* framesOut, ma_uint64 frameCount);
    private:
        ma_engine m_Engine;
        static AudioDevice* s_Instance;
        uint32_t m_Channels = 0;
        uint32_t m_SampleRate = 0;
        Array<Ref<AudioNode>> m_AudioNodes;
    };

    Ref<AudioDevice> CreateAudioDevice(const AudioDeviceCreateInfo& createInfo);
}
