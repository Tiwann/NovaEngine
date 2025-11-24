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

    bool AudioSystem::Initialize(const uint32_t channels, const uint32_t sampleRate, const uint32_t listenerCount)
    {
        ma_allocation_callbacks allocationCallbacks
        {
            this,
            AudioSystemCallbacks::OnMalloc,
            AudioSystemCallbacks::OnRealloc,
            AudioSystemCallbacks::OnFree
        };

        ma_engine_config config = ma_engine_config_init();
        config.channels = channels;
        config.sampleRate = sampleRate;
        config.listenerCount = listenerCount;
        config.allocationCallbacks = allocationCallbacks;
        config.dataCallback = AudioSystemCallbacks::AudioDataProc;
        config.notificationCallback = AudioSystemCallbacks::OnNotification;
        config.onProcess = AudioSystemCallbacks::OnProcess;
        config.pProcessUserData = this;
        config.noAutoStart = true;

        ma_result result = ma_engine_init(&config, &m_Engine);
        MA_RETURN_ON_FAIL(result);

        result = ma_engine_start(&m_Engine);
        MA_RETURN_ON_FAIL(result);

        if (!s_Instance)
        {
            s_Instance = this;
        }
        return true;
    }

    bool AudioSystem::Initialize(AudioSystemCreateInfo createInfo)
    {
        ma_allocation_callbacks allocationCallbacks
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

        ma_result result = ma_engine_init(&config, &m_Engine);
        MA_RETURN_ON_FAIL(result);

        result = ma_engine_start(&m_Engine);
        MA_RETURN_ON_FAIL(result);

        if (!s_Instance)
        {
            s_Instance = this;
        }
        return true;
    }

    void AudioSystem::Destroy()
    {
        ma_engine_stop(&m_Engine);
        ma_engine_uninit(&m_Engine);
    }

    Ref<AudioClip> AudioSystem::CreateClipFromFile(StringView filepath)
    {
        AudioClip* clip = new AudioClip();
        if (!clip->LoadFromFile(filepath))
        {
            delete clip;
            return nullptr;
        }
        return Ref(clip);
    }

    Ref<AudioClip> AudioSystem::CreateClipFromMemory(const uint8_t* data, size_t size)
    {
        AudioClip* clip = new AudioClip();
        if (!clip->LoadFromMemory(data, size))
        {
            delete clip;
            return nullptr;
        }
        return Ref(clip);
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
