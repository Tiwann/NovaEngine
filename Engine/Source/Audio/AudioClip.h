#pragma once
#include "AudioFormat.h"
#include "Runtime/Asset.h"
#include "Containers/StringView.h"
#include "Runtime/Flags.h"


typedef struct ma_sound ma_sound;
typedef struct ma_decoder ma_decoder;

namespace Nova
{
    enum class AudioPlaybackFlagBits
    {
        None = 0,
        FullInMemory = BIT(0),
        Stream = BIT(1),
        Spatialize = BIT(2),
        Looping = BIT(3),

        Music = Stream | Looping,
        SoundEffect = FullInMemory | Spatialize,
    };

    NOVA_DECLARE_FLAGS(AudioPlaybackFlagBits, AudioPlaybackFlags);


    class AudioClip final : public Asset
    {
    public:
        AudioClip();
        ~AudioClip() override;
        bool LoadFromFile(StringView filepath, AudioPlaybackFlags flags);
        bool LoadFromMemory(const void* data, size_t size, AudioPlaybackFlags flags);
        void Destroy();

        AssetType GetAssetType() const override;
        float GetDurationSeconds() const;
        uint64_t GetDurationFrames() const;

        ma_sound* GetHandle();
        const ma_sound* GetHandle() const;

        Array<uint8_t> GetSamples() const;

        AudioFormat GetFormat() const;
    private:
        ma_sound* m_Handle = nullptr;
        ma_decoder* m_Decoder = nullptr;
        Array<uint8_t> m_InternalBuffer;
        AudioPlaybackFlags m_PlaybackFlags = AudioPlaybackFlagBits::None;
    };
}
