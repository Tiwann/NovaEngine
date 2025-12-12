#include "AudioClip.h"
#include "AudioSystem.h"
#include "AudioNode.h"

#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "Containers/StringConversion.h"

namespace Nova
{
    static uint32_t GetMiniaudioPlaybackFlags(const AudioPlaybackFlags flags)
    {
        uint32_t maflags = 0;

        if ((flags & AudioPlaybackFlagBits::Spatialize) != 0)
            maflags |= MA_SOUND_FLAG_NO_SPATIALIZATION;

        if (flags & AudioPlaybackFlagBits::FullInMemory)
            maflags |= MA_SOUND_FLAG_DECODE;

        if (flags & AudioPlaybackFlagBits::Stream)
        {
            maflags &= ~MA_SOUND_FLAG_DECODE;
            maflags |= MA_SOUND_FLAG_STREAM;
        }

        if (flags & AudioPlaybackFlagBits::Looping)
            maflags |= MA_SOUND_FLAG_LOOPING;

        return maflags;
    }

    bool AudioClip::LoadFromFile(const StringView filepath, const AudioPlaybackFlags flags)
    {
        AudioSystem* audioSystem = AudioSystem::GetInstance();

        if (!audioSystem) return false;

        if (m_Initialized)
        {
            ma_sound_uninit(&m_Handle);
            ma_decoder_uninit(&m_Decoder);
            if (m_FFTNode) m_FFTNode->Destroy();
            m_InternalBuffer.Free();
        }

        const ma_decoder_config decoderConfig = ma_decoder_config_init(ma_format_s16, 2, 44100);
        ma_result result = ma_decoder_init_file_w(*StringConvertToWide(filepath), &decoderConfig, &m_Decoder);
        if (result != MA_SUCCESS) return false;

        const uint32_t maflags = GetMiniaudioPlaybackFlags(flags);

        result = ma_sound_init_from_data_source(audioSystem->GetHandle(), &m_Decoder, maflags, nullptr, &m_Handle);
        if (result != MA_SUCCESS) return false;


        if (flags & AudioPlaybackFlagBits::ComputeFFT)
        {
            m_FFTNode = new FFTAudioNode();
            if (!m_FFTNode->Initialize(audioSystem))
                return false;
            if (!audioSystem->AttachAudioNodeToOutputBus(m_FFTNode))
                return false;

            if (!ConnectToNode(m_FFTNode))
                return false;
        }

        m_PlaybackFlags = flags;
        m_Initialized = true;
        return true;
    }

    bool AudioClip::LoadFromMemory(const void* data, const size_t size, const AudioPlaybackFlags flags)
    {
        AudioSystem* audioSystem = AudioSystem::GetInstance();
        if (!audioSystem)
            return false;

        if (m_Initialized)
        {
            ma_sound_uninit(&m_Handle);
            ma_decoder_uninit(&m_Decoder);
            m_FFTNode->Destroy();
            m_InternalBuffer.Free();
        }

        m_InternalBuffer = { (uint8_t*)data, size };

        const ma_decoder_config decoderConfig = ma_decoder_config_init(ma_format_s16, 2, 44100);
        ma_result result = ma_decoder_init_memory(m_InternalBuffer.Data(), m_InternalBuffer.Size(), &decoderConfig, &m_Decoder);
        if (result != MA_SUCCESS) return false;

        const uint32_t maflags = GetMiniaudioPlaybackFlags(flags);

        result = ma_sound_init_from_data_source(audioSystem->GetHandle(), &m_Decoder, maflags, nullptr, &m_Handle);
        if (result != MA_SUCCESS) return false;

        if (flags & AudioPlaybackFlagBits::ComputeFFT)
        {
            m_FFTNode = new FFTAudioNode();
            if (!m_FFTNode->Initialize(audioSystem))
                return false;
            if (!audioSystem->AttachAudioNodeToOutputBus(m_FFTNode))
                return false;

            if (!ConnectToNode(m_FFTNode))
                return false;
        }

        m_Initialized = true;
        m_PlaybackFlags = flags;
        return true;
    }

    void AudioClip::Destroy()
    {
        if (m_Initialized)
        {
            ma_sound_uninit(&m_Handle);
            ma_decoder_uninit(&m_Decoder);
        }
        if (m_FFTNode) m_FFTNode->Destroy();
        m_Initialized = false;
    }

    AssetType AudioClip::GetAssetType() const
    {
        return AssetType::AudioClip;
    }

    ma_sound* AudioClip::GetHandle()
    {
        return &m_Handle;
    }

    const ma_sound* AudioClip::GetHandle() const
    {
        return &m_Handle;
    }

    Array<uint8_t> AudioClip::GetSamples() const
    {
        ma_data_source* source = ma_sound_get_data_source(&m_Handle);
        if (!source) return {};

        const AudioFormat format = GetFormat();
        const uint64_t numFrames = GetDurationFrames();
        const uint64_t numSamples = format.channels * numFrames;
        const uint64_t totalSize = numSamples * format.bytesPerSample;

        Array<uint8_t> samples(totalSize);
        const ma_result result = ma_data_source_read_pcm_frames(source, samples.Data(), numFrames, nullptr);
        if (result != MA_SUCCESS) return {};
        return samples;
    }

    AudioFormat AudioClip::GetFormat() const
    {
        ma_format format;
        ma_uint32 channels;
        ma_uint32 sampleRate;
        const ma_result result = ma_sound_get_data_format(&m_Handle, &format, &channels, &sampleRate, nullptr, 0);
        if (result != MA_SUCCESS) return {};

        uint8_t bytesPerSample = 0;
        switch (format)
        {
        case ma_format_unknown: bytesPerSample = 0; break;
        case ma_format_u8: bytesPerSample = 1; break;
        case ma_format_s16: bytesPerSample = 2; break;
        case ma_format_s24: bytesPerSample = 3; break;
        case ma_format_s32: bytesPerSample = 4; break;
        case ma_format_f32: bytesPerSample = 4; break;
        case ma_format_count: bytesPerSample = 0; break;
        }

        if (bytesPerSample == 0) return {};
        if (channels == 0) return {};
        if (sampleRate == 0) return {};

        return { channels, sampleRate, bytesPerSample, SampleInterleaving::Interleaved };
    }

    bool AudioClip::ConnectToNode(Ref<AudioNode> audioNode)
    {
        const ma_result result = ma_node_attach_output_bus(&m_Handle, 0, audioNode->GetHandle(), 0);
        return result == MA_SUCCESS;
    }

    Ref<FFTAudioNode> AudioClip::GetFFTAudioNode() const
    {
        return m_FFTNode;
    }

    float AudioClip::GetDurationSeconds() const
    {
        float duration = 0.0f;
        const ma_result result = ma_sound_get_length_in_seconds(&m_Handle, &duration);
        if (result != MA_SUCCESS) return 0.0f;
        return duration;
    }

    uint64_t AudioClip::GetDurationFrames() const
    {
        uint64_t frames = 0;
        const ma_result result = ma_sound_get_length_in_pcm_frames(&m_Handle, &frames);
        if (result != MA_SUCCESS) return 0.0f;
        return frames;
    }
}
