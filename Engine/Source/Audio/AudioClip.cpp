#include "AudioClip.h"
#include "AudioSystem.h"

namespace Nova
{
    AudioClip::AudioClip(AudioSystem* inSystem) : m_System(inSystem)
    {
    }

    bool AudioClip::LoadFromFile(const char* filepath)
    {
        ma_engine* engine = m_System->GetHandle();
        const ma_result result = ma_sound_init_from_file(engine, filepath, 0, nullptr, nullptr, &m_Handle);
        if (result != MA_SUCCESS)
            return false;
        return true;
    }

    bool AudioClip::LoadFromMemory(const void* data, size_t size)
    {
        return false;
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