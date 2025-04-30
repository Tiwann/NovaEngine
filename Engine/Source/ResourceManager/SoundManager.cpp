#include "SoundManager.h"
#include "Audio/AudioSystem.h"
#include "Runtime/Application.h"
#include "Runtime/Log.h"


namespace Nova
{
    Sound* SoundManager::Load(const String& Name, const Path& Filepath, SoundFlags Flags)
    {
        Sound* LoadedSound = Load(Name, Filepath);
        if(!LoadedSound) return nullptr;
        LoadedSound->SetFlags(Flags);
        return LoadedSound;
    }

    Sound* SoundManager::Load(const String& Name, const Path& Filepath)
    {
        Sound* LoadedSound = Sound::CreateFromFile(Filepath, SoundFlagBits::Default);
        m_Data[Name] = LoadedSound;
        return LoadedSound;
    }

    Sound* SoundManager::Retrieve(const String& Name)
    {
        if(!m_Data.Contains(Name))
        {
            NOVA_LOG(SoundManager, Verbosity::Warning, "Sound \"{}\" not found!", Name);
            return nullptr;
        }
        return m_Data[Name];
    }
}
