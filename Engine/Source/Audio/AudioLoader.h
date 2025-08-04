#pragma once
#include "AudioFormat.h"

#include <miniaudio.h>

namespace Nova
{
    typedef struct uint24 { uint32_t value : 24; } uint24_t;
    class AudioLoader
    {
    public:
        template<AudioChannels C>
        static AudioBuffer<float, C> LoadAudioSample(const char* filepath, const uint32_t channels = (uint32_t)C, const uint32_t sampleRate = 44100)
        {
            const ma_decoder_config config = ma_decoder_config_init(ma_format_f32, channels, sampleRate);
            ma_decoder decoder;
            ma_decoder_init_file(filepath, &config, &decoder);

            AudioFormat format(channels, 44100, channels * sizeof(float), SampleInterleaving::Interleaved);

            ma_uint64 length;
            ma_decoder_get_length_in_pcm_frames(&decoder, &length);

            const size_t size = length * channels * sizeof(float);
            uint8_t* data = new uint8_t[size];
            ma_uint64 read = ~0;
            ma_decoder_read_pcm_frames(&decoder, data, length, &read);

            return AudioBuffer<float, C>(format, (float*)data, size);
        }

        template<AudioChannels C>
        static AudioBuffer<int32_t, C> LoadAudioSample(const char* filepath, const uint32_t channels = (uint32_t)C, const uint32_t sampleRate = 44100)
        {
            const ma_decoder_config config = ma_decoder_config_init(ma_format_s32, channels, sampleRate);
            ma_decoder decoder;
            ma_decoder_init_file(filepath, &config, &decoder);

            AudioFormat format(channels, 44100, channels * sizeof(int32_t), SampleInterleaving::Interleaved);

            ma_uint64 length;
            ma_decoder_get_length_in_pcm_frames(&decoder, &length);

            const size_t size = length * channels * sizeof(int32_t);
            uint8_t* data = new uint8_t[size];
            ma_uint64 read = ~0;
            ma_decoder_read_pcm_frames(&decoder, data, length, &read);

            return AudioBuffer<int32_t, C>(format, (int32_t*)data, size);
        }

        template<AudioChannels C>
        static AudioBuffer<int16_t, C> LoadAudioSample(const char* filepath, const uint32_t channels = (uint32_t)C, const uint32_t sampleRate = 44100)
        {
            const ma_decoder_config config = ma_decoder_config_init(ma_format_s16, channels, sampleRate);
            ma_decoder decoder;
            ma_decoder_init_file(filepath, &config, &decoder);

            AudioFormat format(channels, 44100, channels * sizeof(int16_t), SampleInterleaving::Interleaved);

            ma_uint64 length;
            ma_decoder_get_length_in_pcm_frames(&decoder, &length);

            const size_t size = length * channels * sizeof(int16_t);
            uint8_t* data = new uint8_t[size];
            ma_uint64 read = ~0;
            ma_decoder_read_pcm_frames(&decoder, data, length, &read);

            return AudioBuffer<int16_t, C>(format, (int16_t*)data, size);
        }

        template<AudioChannels C>
        static AudioBuffer<uint24_t, C> LoadAudioSample(const char* filepath, const uint32_t channels = (uint32_t)C, const uint32_t sampleRate = 44100)
        {
            const ma_decoder_config config = ma_decoder_config_init(ma_format_s16, channels, sampleRate);
            ma_decoder decoder;
            ma_decoder_init_file(filepath, &config, &decoder);

            AudioFormat format(channels, 44100, channels * 3, SampleInterleaving::Interleaved);

            ma_uint64 length;
            ma_decoder_get_length_in_pcm_frames(&decoder, &length);

            const size_t size = length * channels * 3;
            uint8_t* data = new uint8_t[size];
            ma_uint64 read = ~0;
            ma_decoder_read_pcm_frames(&decoder, data, length, &read);

            return AudioBuffer<int16_t, C>(format, (uint24_t*)data, size);
        }
    };
}
