#include "AudioDevice.h"
#include "AudioClip.h"
#include "Runtime/Memory.h"
#include <cstring>
#include <print>

#include "AudioNode.h"

#define MA_FAILED(result) ((result) != MA_SUCCESS)
#define MA_RETURN_ON_FAIL(result) if(MA_FAILED((result))) return false
#define MA_RETURN_ON_FAIL_DATA(result, data) if(MA_FAILED((result))) return (data)

struct AudioDeviceCallbacks
{

};


namespace Nova
{
    AudioDevice* AudioDevice::s_Instance = nullptr;

    AudioDevice::AudioDevice()
    {
        memset(&m_Engine, 0, sizeof(ma_engine));
    }

    bool AudioDevice::Initialize(const AudioDeviceCreateInfo& createInfo)
    {
        const ma_allocation_callbacks allocationCallbacks
        {
            this,
            AudioDevice::OnMalloc,
            AudioDevice::OnRealloc,
            AudioDevice::OnFree
        };

        ma_engine_config config = ma_engine_config_init();
        config.channels = createInfo.channels;
        config.sampleRate = createInfo.sampleRate;
        config.listenerCount = createInfo.listenerCount;
        config.allocationCallbacks = allocationCallbacks;
        config.dataCallback = AudioDevice::AudioDataProc;
        config.notificationCallback = AudioDevice::OnNotification;
        config.onProcess = AudioDevice::OnProcess;
        config.pProcessUserData = this;
        config.noAutoStart = true;

        ma_engine_stop(&m_Engine);
        ma_engine_uninit(&m_Engine);
        ma_result result = ma_engine_init(&config, &m_Engine);
        MA_RETURN_ON_FAIL(result);

        result = ma_engine_start(&m_Engine);
        MA_RETURN_ON_FAIL(result);

        m_Channels = createInfo.channels;
        m_SampleRate = createInfo.sampleRate;
        s_Instance = this;
        return true;
    }

    void AudioDevice::Destroy()
    {
        ma_engine_stop(&m_Engine);
        ma_engine_uninit(&m_Engine);
    }


    void AudioDevice::PlayAudioClip(AudioClip* clip)
    {
        if (!clip) return;
        ma_sound_start(clip->GetHandle());
    }

    void AudioDevice::StopAudioClip(AudioClip* clip)
    {
        if (!clip) return;
        ma_sound_stop(clip->GetHandle());
    }

    AudioDevice* AudioDevice::GetInstance()
    {
        return s_Instance;
    }

    ma_engine* AudioDevice::GetInstanceHandle()
    {
        return s_Instance->GetHandle();
    }

    uint32_t AudioDevice::GetOutputChannelCount() const
    {
        return m_Channels;
    }

    uint32_t AudioDevice::GetOutputSampleRate() const
    {
        return m_SampleRate;
    }

    AudioFormat AudioDevice::GetOutputFormat() const
    {
        static auto GeBytesPerSample = [](const ma_format format) -> uint32_t
        {
            switch (format)
            {
            case ma_format_unknown: return 0;
            case ma_format_u8: return 1;
            case ma_format_s16: return 2;
            case ma_format_s24: return 3;
            case ma_format_s32: return 4;
            case ma_format_f32: return 4;
            case ma_format_count: return 0;
            }
            return 0;
        };

        const ma_device* device = ma_engine_get_device((ma_engine*)&m_Engine);

        return {m_Channels, m_SampleRate, GeBytesPerSample(device->playback.format), SampleInterleaving::Interleaved};
    }

    bool AudioDevice::AttachAudioNodeToOutputBus(Ref<AudioNode> audioNode)
    {
        const ma_result result = ma_node_attach_output_bus(audioNode->GetHandle(), 0, ma_engine_get_endpoint(&m_Engine), 0);
        return result == MA_SUCCESS;
    }

    void AudioDevice::DetachAudioNodeFromOutputBus(const Ref<AudioNode>& audioNode)
    {
        //ma_node_detach_output_bus(audioNode.Get(), 0, ma_engine_get_endpoint(&m_Engine), 0);
    }

    void* AudioDevice::OnMalloc(const size_t size, void* userData)
    {
        (void)userData;
        return Memory::Malloc<uint8_t>(size);
    }

    void* AudioDevice::OnRealloc(void* where, const size_t size, void* userData)
    {
        (void)userData;
        return Memory::Realloc<uint8_t>((uint8_t*)where, size);
    }

    void AudioDevice::OnFree(void* ptr, void* userData)
    {
        Memory::Free(ptr);
        (void)userData;
    }

    void AudioDevice::AudioDataProc(ma_device* device, void* output, const void* input, ma_uint32 frameCount)
    {
        ma_engine* engine = (ma_engine*)device->pUserData;
        ma_engine_read_pcm_frames(engine, output, frameCount, nullptr);
    }

    void AudioDevice::OnNotification(const ma_device_notification* notification)
    {

    }

    void AudioDevice::OnProcess(void* userData, float* framesOut, ma_uint64 frameCount)
    {

    }

    Ref<AudioDevice> CreateAudioDevice(const AudioDeviceCreateInfo& createInfo)
    {
        AudioDevice* audioSystem = new AudioDevice();
        if (!audioSystem->Initialize(createInfo))
        {
            delete audioSystem;
            return nullptr;
        }
        return Ref(audioSystem);
    }
}
