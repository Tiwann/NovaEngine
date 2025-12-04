#pragma once
#include "AudioFormat.h"
#include "FFTAudioNode.h"
#include "Runtime/Asset.h"
#include "Containers/StringView.h"
#include "Runtime/Flags.h"
#include "Runtime/Ref.h"

#include <miniaudio.h>

namespace Nova
{
    enum class AudioPlaybackFlagBits
    {
        None = 0,
        FullInMemory = BIT(0),
        Stream = BIT(1),
        Spatialize = BIT(2),
        Looping = BIT(3),
        ComputeFFT = BIT(4),

        Music = Stream | Looping,
        SoundEffect = FullInMemory | Spatialize,
    };

    NOVA_DECLARE_FLAGS(AudioPlaybackFlagBits, AudioPlaybackFlags);

    class AudioNode;

    class AudioClip final : public Asset
    {
    public:
        AudioClip() = default;
        ~AudioClip() override = default;
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

        bool ConnectToNode(Ref<AudioNode> audioNode);
        Ref<FFTAudioNode> GetFFTAudioNode() const;

        bool IsValid() const { return m_Initialized;}
    private:
        ma_sound m_Handle;
        ma_decoder m_Decoder;
        Array<uint8_t> m_InternalBuffer;
        AudioPlaybackFlags m_PlaybackFlags = AudioPlaybackFlagBits::None;
        Ref<FFTAudioNode> m_FFTNode = nullptr;
        bool m_Initialized = false;
    };
}
