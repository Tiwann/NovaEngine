#pragma once

#include <miniaudio.h>

namespace Nova
{
    class AudioClip
    {
    public:
        bool LoadFromFile(const char* filepath);
        bool LoadFromMemory(const void* data, size_t size);

        ma_sound* GetHandle();
        const ma_sound* GetHandle() const;
    private:
        friend class AudioSystem;
        explicit AudioClip(AudioSystem* inSystem);
    private:
        AudioSystem* m_System = nullptr;
        ma_sound m_Handle;
    };
}