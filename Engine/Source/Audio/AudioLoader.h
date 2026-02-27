#pragma once
#include "AudioFormat.h"
#include "Containers/StringView.h"
#include "Runtime/FileUtils.h"

#include <miniaudio.h>

namespace Nova
{
    template<AudioSample T>
    ma_format GetMiniaudioFormat()
    {
        if constexpr (typeid(T) == typeid(float))
            return ma_format_f32;
        else if constexpr (typeid(T) == typeid(double))
            return ma_format_unknown;
        else if constexpr (typeid(T) == typeid(int32_t))
            return ma_format_s32;
        else if constexpr (typeid(T) == typeid(int24_t))
            return ma_format_s24;
        else if constexpr (typeid(T) == typeid(int16_t))
            return ma_format_s16;
        else if constexpr (typeid(T) == typeid(uint8_t))
            return ma_format_u8;
        return ma_format_unknown;
    }

    class AudioLoader
    {
    public:
        template<AudioSample T, AudioChannels C>
        static AudioBuffer<T, C> LoadAudioToBuffer(const uint8_t* data, size_t dataSize, const uint32_t channels = (uint32_t)C, const uint32_t sampleRate = 44100)
        {
            const ma_decoder_config config = ma_decoder_config_init(GetMiniaudioFormat<T>(), channels, sampleRate);
            ma_decoder decoder;
            ma_decoder_init_memory(data, dataSize, &config, &decoder);

            ma_uint64 length;
            ma_decoder_get_length_in_pcm_frames(&decoder, &length);
            if (length == 0)
                return {};

            AudioFormat format(channels, sampleRate, channels * sizeof(T), SampleInterleaving::Interleaved);
            const size_t size = length * channels * sizeof(T);
            AudioBuffer<T, C> buffer = AudioBuffer<T, C>(format, size);
            ma_uint64 read = ~0;
            ma_decoder_read_pcm_frames(&decoder, buffer.GetWritePointer(), length, &read);
            if (read != length)
                return {};
            return buffer;
        }

        template<AudioSample T, AudioChannels C>
        static AudioBuffer<T, C> LoadAudioToBuffer(const StringView filepath, const uint32_t channels = (uint32_t)C, const uint32_t sampleRate = 44100)
        {
            const Array<uint8_t> fileContent = FileUtils::ReadToBuffer(filepath);
            return LoadAudioToBuffer<T, C>(fileContent.Data(), fileContent.Size(), channels, sampleRate);
        }
    };
}
