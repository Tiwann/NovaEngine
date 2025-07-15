#pragma once
#include "AudioFormat.h"

#include <miniaudio.h>

namespace Nova
{
    class AudioLoader
    {
    public:
        template<AudioChannels C>
        static AudioBuffer<float, C> LoadAudioSample(const char* filepath, const uint32_t channels = (uint32_t)C, const uint32_t sampleRate = 44100)
        {
            const ma_decoder_config config = ma_decoder_config_init(ma_format_f32, channels, sampleRate);
            ma_decoder decoder;
            ma_decoder_init_file(filepath, &config, &decoder);

            AudioFormat format(channels, 44100, channels * 4, SampleInterleaving::Interleaved);

            ma_uint64 length;
            ma_decoder_get_length_in_pcm_frames(&decoder, &length);

            const size_t size = length * channels * sizeof(float);
            uint8_t* data = new uint8_t[size];
            ma_uint64 read = 0xFF;
            ma_decoder_read_pcm_frames(&decoder, data, length, &read);

            return AudioBuffer<float, C>(format, (float*)data, size);
        }
    private:

    };
}
