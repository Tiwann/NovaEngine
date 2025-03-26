#include "Sound.h"
#include "AudioSystem.h"
#include "Core/Application.h"

#include <fmod/fmod.hpp>


namespace Hydro
{
    Sound::Sound(const String& Name) : Asset(Name),
          m_Flags(SoundFlagBits::Default),
          m_Type(SoundType::Unknown),
          m_Format(SoundFormat::None),
          m_Channels(0),
          m_BitsPerSample(0),
          m_Duration(0)
    {
    }

    Sound::~Sound()
    {
        if (m_Handle)
            m_Handle->release();
    }

    Sound* Sound::CreateFromFile(const Path& Filepath, SoundFlags Flags)
    {
        Sound* NewSound = new Sound;
        if(!NewSound->LoadData(Filepath, Flags))
        {
            delete NewSound;
            return nullptr;
        }
        return NewSound;
    }

    Sound* Sound::Create()
    {
        return new Sound;
    }

    String Sound::GetAssetType() const
    {
        return "Sound";
    }

    bool Sound::LoadData(const Path& Filepath, SoundFlags Flags)
    {
        AudioSystem* AudioSystem = g_Application->GetAudioSystem();
        if (!AudioSystem) return false;
        if (m_Handle)
        {
            m_Handle->release();
            m_Handle = nullptr;
        }

        m_Flags = Flags;
        FMOD::Sound* LoadedSound = AudioSystem->CreateSound(Filepath, Flags);
        if(!LoadedSound) return false;
        m_Handle = LoadedSound;
        
        m_Handle->getFormat((FMOD_SOUND_TYPE*)&m_Type,
            (FMOD_SOUND_FORMAT*)&m_Format,
            (int*)&m_Channels,
            (int*)&m_BitsPerSample);

        m_Handle->getDefaults(&m_SampleRate, nullptr);

        u32 Length;
        m_Handle->getLength(&Length, FMOD_TIMEUNIT_MS);
        m_Duration = (f32)Length / 1000.0f;
        
        return true;
    }

    bool Sound::LoadData(const BufferView<u8>& Buffer, SoundFlags Flags)
    {
        AudioSystem* AudioSystem = g_Application->GetAudioSystem();
        if (!AudioSystem) return false;
        if (m_Handle)
        {
            m_Handle->release();
            m_Handle = nullptr;
        }

        m_Flags = Flags;
        FMOD::Sound* LoadedSound = AudioSystem->CreateSound(Buffer, Flags);
        if(!LoadedSound) return false;
        m_Handle = LoadedSound;
        
        m_Handle->getFormat((FMOD_SOUND_TYPE*)&m_Type,
            (FMOD_SOUND_FORMAT*)&m_Format,
            (int*)&m_Channels,
            (int*)&m_BitsPerSample);
        
        m_Handle->getDefaults(&m_SampleRate, nullptr);

        u32 Length;
        m_Handle->getLength(&Length, FMOD_TIMEUNIT_MS);
        m_Duration = (f32)Length / 1000.0f;
        return true;
    }

    FMOD::Sound* Sound::GetHandle() const
    {
        return m_Handle;
    }

    SoundFlags Sound::GetFlags() const
    {
        return m_Flags;
    }

    void Sound::SetFlags(SoundFlags Flags)
    {
        m_Handle->setMode(Flags.As<FMOD_MODE>());
        m_Flags = Flags;
    }

    SoundType Sound::GetType() const
    {
        return m_Type;
    }

    SoundFormat Sound::GetFormat() const
    {
        return m_Format;
    }

    u32 Sound::GetChannels() const
    {
        return m_Channels;
    }

    u32 Sound::GetBitsPerSample() const
    {
        return m_BitsPerSample;
    }

    f32 Sound::GetDuration() const
    {
        return m_Duration;
    }

    float Sound::GetSampleRate() const
    {
        return m_SampleRate;
    }

    void Sound::SetLoopPoints(u32 StartPointPCM, u32 EndPointPCM)
    {
        m_LoopStartPCM = StartPointPCM;
        m_LoopEndPCM = EndPointPCM;
        m_Handle->setLoopPoints(StartPointPCM, FMOD_TIMEUNIT_PCM, EndPointPCM, FMOD_TIMEUNIT_PCM);
    }

    void Sound::GetLoopPoints(u32& OutStartPointPCM, u32& OutEndPointPCM) const
    {
        OutStartPointPCM = m_LoopStartPCM;
        OutEndPointPCM = m_LoopEndPCM;
    }
}
