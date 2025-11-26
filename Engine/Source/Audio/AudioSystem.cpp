#include "AudioSystem.h"
#include "AudioClip.h"
#include "Runtime/Memory.h"
#include <cstdlib>
#include <cstring>
#include <print>

#define MA_FAILED(result) ((result) != MA_SUCCESS)
#define MA_RETURN_ON_FAIL(result) if(MA_FAILED((result))) return false
#define MA_RETURN_ON_FAIL_DATA(result, data) if(MA_FAILED((result))) return (data)

struct AudioSystemCallbacks
{
    static void* OnMalloc(const size_t size, void* userData)
    {
        (void)userData;
        return Nova::Memory::Malloc<uint8_t>(size);
    }

    static void* OnRealloc(void* where, const size_t size, void* userData)
    {
        (void)userData;
        return Nova::Memory::Realloc<uint8_t>((uint8_t*)where, size);
    }

    static void OnFree(void* ptr, void* userData)
    {
        Nova::Memory::Free(ptr);
        (void)userData;
    }

    static void AudioDataProc(ma_device* device, void* output, const void* input, ma_uint32 frameCount)
    {
        ma_engine* engine = (ma_engine*)device->pUserData;
        ma_engine_read_pcm_frames(engine, output, frameCount, nullptr);
    }

    static void OnNotification(const ma_device_notification* notification)
    {

    }

    static void OnProcess(void* userData, float* framesOut, ma_uint64 frameCount)
    {

    }
};




namespace Nova
{
    AudioSystem* AudioSystem::s_Instance = nullptr;

    AudioSystem::AudioSystem() : Object("AudioSystem")
    {
        memset(&m_Engine, 0, sizeof(ma_engine));
    }

    bool AudioSystem::Initialize(const AudioSystemCreateInfo& createInfo)
    {
        const ma_allocation_callbacks allocationCallbacks
        {
            this,
            AudioSystemCallbacks::OnMalloc,
            AudioSystemCallbacks::OnRealloc,
            AudioSystemCallbacks::OnFree
        };

        ma_engine_config config = ma_engine_config_init();
        config.channels = createInfo.channels;
        config.sampleRate = createInfo.sampleRate;
        config.listenerCount = createInfo.listenerCount;
        config.allocationCallbacks = allocationCallbacks;
        config.dataCallback = AudioSystemCallbacks::AudioDataProc;
        config.notificationCallback = AudioSystemCallbacks::OnNotification;
        config.onProcess = AudioSystemCallbacks::OnProcess;
        config.pProcessUserData = this;
        config.noAutoStart = true;

        Destroy();
        ma_result result = ma_engine_init(&config, &m_Engine);
        MA_RETURN_ON_FAIL(result);

        result = ma_engine_start(&m_Engine);
        MA_RETURN_ON_FAIL(result);

        m_Channels = createInfo.channels;
        m_SampleRate = createInfo.sampleRate;
        s_Instance = this;
        return true;
    }

    void AudioSystem::Destroy()
    {
        ma_engine_stop(&m_Engine);
        ma_engine_uninit(&m_Engine);
    }


    void AudioSystem::PlayAudioClip(AudioClip* clip)
    {
        ma_sound_start(clip->GetHandle());
    }

    void AudioSystem::StopAudioClip(AudioClip* clip)
    {
        ma_sound_stop(clip->GetHandle());
    }

    AudioSystem* AudioSystem::GetInstance()
    {
        return s_Instance;
    }

    ma_engine* AudioSystem::GetInstanceHandle()
    {
        return s_Instance->GetHandle();
    }

    uint32_t AudioSystem::GetOutputChannelCount() const
    {
        return m_Channels;
    }

    uint32_t AudioSystem::GetOutputSampleRate() const
    {
        return m_SampleRate;
    }

    AudioFormat AudioSystem::GetOutputFormat() const
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

    Ref<AudioSystem> CreateAudioSystem(const AudioSystemCreateInfo& createInfo)
    {
        AudioSystem* audioSystem = new AudioSystem();
        if (!audioSystem->Initialize(createInfo))
        {
            delete audioSystem;
            return nullptr;
        }
        return Ref(audioSystem);
    }
}
