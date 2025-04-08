#pragma once
#include "SoundOptions.h"
#include "Runtime/Filesystem.h"
#include "Runtime/LogCategory.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(AudioEngine, "AUDIO ENGINE")
NOVA_DECLARE_LOG_CATEGORY_STATIC(FMOD, "FMOD")

namespace FMOD
{
    class System;
    class Sound;
}

namespace Nova
{
    class Application;
    
    class AudioSystem
    {
    public:
        AudioSystem(Application* Owner) : m_Application(Owner) {}
        ~AudioSystem() = default;
        
        bool Init();
        void Destroy();
        void OnUpdate();

        FMOD::Sound* CreateSound(const Path& Filepath, SoundFlags Flags);
        FMOD::Sound* CreateSound(const BufferView<u8>& Buffer, SoundFlags Flags);
        FMOD::System* GetHandle() const;

    private:
        Application* m_Application = nullptr;
        FMOD::System* m_Handle = nullptr;
    };
}
