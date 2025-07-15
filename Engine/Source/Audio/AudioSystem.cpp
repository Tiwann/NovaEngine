#include "AudioSystem.h"
#include <cstdlib>
#include <cstring>
#include <print>

#include "AudioClip.h"

#define MA_FAILED(result) (result != MA_SUCCESS)

static void* OnMalloc(const size_t size, void* userData)
{
    return malloc(size);
}

static void* OnRealloc(void* where, const size_t size, void* userData)
{
    return realloc(where, size);
}

static void OnFree(void* ptr, void* userData)
{
    free(ptr);
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


namespace Nova
{
    AudioSystem::AudioSystem()
    {
        memset(&m_Engine, 0, sizeof(ma_engine));
    }

    bool AudioSystem::Initialize(const uint32_t Channels, const uint32_t SampleRate, const uint32_t ListenerCount)
    {
        ma_engine_config config = ma_engine_config_init();
        config.channels = Channels;
        config.sampleRate = SampleRate;
        config.listenerCount = ListenerCount;
        config.allocationCallbacks = ma_allocation_callbacks(this, OnMalloc, OnRealloc, OnFree);
        config.dataCallback = AudioDataProc;
        config.notificationCallback = OnNotification;
        config.onProcess = OnProcess;
        config.pProcessUserData = this;
        config.noAutoStart = true;

        ma_result result = ma_engine_init(&config, &m_Engine);
        if (result != MA_SUCCESS) return false;

        result = ma_engine_start(&m_Engine);
        if (result != MA_SUCCESS) return false;

        return true;
    }

    void AudioSystem::Shutdown()
    {
        ma_engine_stop(&m_Engine);
        ma_engine_uninit(&m_Engine);
    }

    AudioClip* AudioSystem::CreateClipFromFile(const char* filepath)
    {
        AudioClip* clip = new AudioClip(this);
        if (!clip->LoadFromFile(filepath))
        {
            delete clip;
            return nullptr;
        }
        return clip;
    }

    AudioClip* AudioSystem::CreateClipFromMemory(const uint8_t* data, size_t size)
    {
        AudioClip* clip = new AudioClip(this);
        if (!clip->LoadFromMemory(data, size))
        {
            delete clip;
            return nullptr;
        }
        return clip;
    }

    void AudioSystem::PlayAudioClip(AudioClip* clip)
    {
        ma_sound_start(clip->GetHandle());
    }

    void AudioSystem::StopAudioClip(AudioClip* clip)
    {
        ma_sound_stop(clip->GetHandle());
    }

    void AudioSystem::PauseAudioClip(AudioClip* clip)
    {
        ma_sound_stop(clip->GetHandle());
    }
}
