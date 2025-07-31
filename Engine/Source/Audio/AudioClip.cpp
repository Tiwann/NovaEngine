#include "AudioClip.h"
#include "AudioSystem.h"

namespace Nova
{
    bool AudioClip::LoadFromFile(const StringView filepath)
    {
        AudioSystem* audioSystem = AudioSystem::GetInstance();
        if (!audioSystem)
            return false;

        const ma_result result = ma_sound_init_from_file(audioSystem->GetHandle(), *filepath, 0, nullptr, nullptr, &m_Handle);
        if (result != MA_SUCCESS)
            return false;
        return true;
    }

    bool AudioClip::LoadFromMemory(const void* data, size_t size)
    {
        return false;
    }

    void AudioClip::Destroy()
    {
        ma_sound_uninit(&m_Handle);
    }

    String AudioClip::GetAssetType() const
    {
        return "Audio Clip";
    }

    ma_sound* AudioClip::GetHandle()
    {
        return &m_Handle;
    }

    const ma_sound* AudioClip::GetHandle() const
    {
        return &m_Handle;
    }


}