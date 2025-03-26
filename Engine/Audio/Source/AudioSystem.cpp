#include "AudioSystem.h"
#include "Core/Flags.h"
#include "Core/Log.h"
#include "Core/LogVerbosity.h"
#include "Core/Containers/String.h"
#include "Core/Containers/StringFormat.h"

#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>

#include "Core/Application.h"


#define FMOD_FAILED(Result) (Result) != FMOD_RESULT::FMOD_OK

#define FMOD_CHECK(Result) \
    if(FMOD_FAILED((Result))) \
    { \
        Hydro::String Error = FMOD_ErrorString((Result)); \
        HYDRO_LOG(AudioEngine, Verbosity::Error, "Failed to create audio engine: {}", Error); \
        return false; \
    }((void)0)

namespace FMOD
{
    enum class DebugFlagBits : FMOD_DEBUG_FLAGS
    {
        LevelNone = FMOD_DEBUG_LEVEL_NONE,
        LevelError = FMOD_DEBUG_LEVEL_ERROR,
        LevelWarning = FMOD_DEBUG_LEVEL_WARNING,
        LevelLog = FMOD_DEBUG_LEVEL_LOG,
        TypeMemory = FMOD_DEBUG_TYPE_MEMORY,
        TypeFile = FMOD_DEBUG_TYPE_FILE,
        TypeCodec = FMOD_DEBUG_TYPE_CODEC,
        TypeTrace = FMOD_DEBUG_TYPE_TRACE,
        DisplayTimestamps = FMOD_DEBUG_DISPLAY_TIMESTAMPS,
        DisplayLineNumbers = FMOD_DEBUG_DISPLAY_LINENUMBERS,
        DisplayThread = FMOD_DEBUG_DISPLAY_THREAD,
    };

    HYDRO_DECLARE_FLAGS(DebugFlagBits, DebugFlags);
}
        
namespace Hydro
{
    static FMOD_RESULT FMODDebugCallback(FMOD_DEBUG_FLAGS flags, const char *file, int line,const char *func, const char *message)
    {
        const FMOD::DebugFlags Flags(flags);
        if(Flags.Contains(FMOD::DebugFlagBits::LevelError))
        {
            HYDRO_LOG(FMOD, Verbosity::Error, "An error occured: File {}. Line {}. Func {}: {}",
                String(file), line, String(func), String(message));
        }

        if(Flags.Contains(FMOD::DebugFlagBits::LevelWarning))
        {
            HYDRO_LOG(FMOD, Verbosity::Warning, "Warning: File {}. Line {}. Func {}: {}",
                String(file), line, String(func), String(message));
        }

        return FMOD_OK;
    }

    bool AudioSystem::Init()
    {
        FMOD_RESULT Result = FMOD::System_Create(&m_Handle);
        FMOD_CHECK(Result);

#if defined(HYDRO_DEBUG)
        const FMOD::DebugFlags Flags = FMOD::DebugFlagBits::LevelError | FMOD::DebugFlagBits::LevelWarning;
        FMOD::Debug_Initialize(Flags.As<FMOD_DEBUG_FLAGS>(), FMOD_DEBUG_MODE_CALLBACK, FMODDebugCallback);
#endif
        Result = m_Handle->setOutput(FMOD_OUTPUTTYPE_AUTODETECT);
        FMOD_CHECK(Result);

        const AudioSettings& Settings = m_Application->GetAudioSettings();
        Result = m_Handle->setDSPBufferSize(Settings.BufferSize, (int)Settings.BufferCount);
        FMOD_CHECK(Result);

        Result = m_Handle->set3DNumListeners(FMOD_MAX_LISTENERS);
        FMOD_CHECK(Result);

        Result = m_Handle->setSoftwareFormat((int)Settings.SampleRate, FMOD_SPEAKERMODE_STEREO, 0);
        FMOD_CHECK(Result);

        Result = m_Handle->init(1024, FMOD_INIT_NORMAL, nullptr);
        FMOD_CHECK(Result);
        
        return true;
    }

    void AudioSystem::Destroy()
    {
        m_Handle->close();
        m_Handle->release();
    }

    void AudioSystem::OnUpdate()
    {
        m_Handle->update();
    }

    FMOD::Sound* AudioSystem::CreateSound(const Path& Filepath, SoundFlags Flags)
    {
        FMOD::Sound* Sound = nullptr;

        const SoundFlags NewFlags = Flags.Append(SoundFlags(FMOD_CREATESAMPLE));
        const FMOD_RESULT Result = m_Handle->createSound(Filepath.string().c_str(), NewFlags.As<FMOD_MODE>(), nullptr, &Sound);
        if(Result != FMOD_OK)
        {
            const String Error = FMOD_ErrorString(Result);
            HYDRO_LOG(AudioEngine, Verbosity::Error, "Failed to create sound: {}", Error);
            return nullptr;
        }
        return Sound;
    }

    FMOD::Sound* AudioSystem::CreateSound(const BufferView<u8>& Buffer, SoundFlags Flags)
    {
        FMOD::Sound* Sound = nullptr;
        FMOD_CREATESOUNDEXINFO SoundCreateInfo = {};
        SoundCreateInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
        SoundCreateInfo.length = (u32)Buffer.Count();

        SoundFlags NewFlags = Flags.Append(SoundFlags(FMOD_CREATESAMPLE)).Append(SoundFlags(FMOD_OPENMEMORY));
        const FMOD_RESULT Result = m_Handle->createSound((const char*)Buffer.Data(), NewFlags.As<FMOD_MODE>(), &SoundCreateInfo, &Sound);
        if(Result != FMOD_OK)
        {
            const String Error = FMOD_ErrorString(Result);
            HYDRO_LOG(AudioEngine, Verbosity::Error, "Failed to create sound: {}", Error);
            return nullptr;
        }
        return Sound;
    }


    FMOD::System* AudioSystem::GetHandle() const
    {
        return m_Handle;
    }
}
