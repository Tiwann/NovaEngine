#pragma once
#include "SoundFormat.h"
#include "SoundType.h"
#include "Runtime/Filesystem.h"
#include "Audio/SoundOptions.h"
#include "Runtime/Asset.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(AudioClip, "AUDIO CLIP")

namespace FMOD
{
    class Sound;
}

namespace Nova
{
    class Sound : public Asset
    {
    public:
        Sound(const String& Name = "NewSound");
        ~Sound() override;
        

        static Sound* CreateFromFile(const Path& Filepath, SoundFlags Flags);
        static Sound* Create();
        String GetAssetType() const override;

        bool LoadData(const Path& Filepath, SoundFlags Flags);
        bool LoadData(const BufferView<u8>& Buffer, SoundFlags Flags);
        FMOD::Sound* GetHandle() const;
        SoundFlags GetFlags() const;
        
        void SetFlags(SoundFlags Flags);
        SoundType GetType() const;
        SoundFormat GetFormat() const;
        u32 GetChannels() const;
        u32 GetBitsPerSample() const;
        f32 GetDuration() const;
        float GetSampleRate() const;
        void SetLoopPoints(u32 StartPointPCM, u32 EndPointPCM);
        void GetLoopPoints(u32& OutStartPointPCM, u32& OutEndPointPCM) const;
    private:
        
        friend class AudioSystem;
        FMOD::Sound* m_Handle = nullptr;
        SoundFlags m_Flags;
        SoundType m_Type;
        SoundFormat m_Format;
        f32 m_SampleRate = 0;
        u32 m_Channels = 0;
        u32 m_BitsPerSample = 0;
        f32 m_Duration = 0;
        u32 m_LoopStartPCM = 0, m_LoopEndPCM = 0;
    };
}
