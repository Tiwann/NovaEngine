#pragma once
#include <cassert>
#include <complex.h>
#include <cstdint>
#include <type_traits>

namespace Nova
{
    enum class SampleInterleaving
    {
        Interleaved,
        Contiguous
    };

    struct AudioFormat
    {
        uint32_t channels;
        uint32_t sampleRate;
        uint32_t bitsPerSample;
        SampleInterleaving interleaving;
    };

    enum class AudioChannels
    {
        None = 0,
        Mono = 1,
        Stereo = 2,
        Surround_5_1 = 5,
        Surround_7_1 = 7,
    };

    template<typename T, AudioChannels> requires std::is_floating_point_v<T>
    struct AudioFrame;

    template<typename T> requires std::is_floating_point_v<T>
    struct AudioFrame<T, AudioChannels::Mono>
    {
        T center;
    };

    template<typename T> requires std::is_floating_point_v<T>
    struct AudioFrame<T, AudioChannels::Stereo>
    {
        T left, right;
    };

    template <typename T> requires std::is_floating_point_v<T>
    struct AudioFrame<T, AudioChannels::Surround_5_1>
    {
        T left, right, subwoofer, leftSurround, rightSurround;
    };

    template <typename T> requires std::is_floating_point_v<T>
    struct AudioFrame<T, AudioChannels::Surround_7_1>
    {
        T left, right, subwoofer, leftSurround, rightSurround, leftBackSurround, rightBackSurround;
    };


    template<typename T, AudioChannels C = AudioChannels::Stereo> requires std::is_floating_point_v<T>
    class AudioBuffer
    {
    public:
        explicit AudioBuffer(const AudioFormat& format, T* data, size_t size) : m_Format(format), m_Data((uint8_t*)data), m_Size(size)
        {
            assert(format.channels == (uint32_t)C);
        }


        AudioFrame<T, C> GetFrame(size_t Index)
        {
            AudioFrame<T, C> frame;
            if constexpr (C == AudioChannels::Mono)
            {
                frame.center = m_Data[Index];
                return frame;
            }

            if constexpr (C == AudioChannels::Stereo)
            {
                if (m_Format.interleaving == SampleInterleaving::Interleaved)
                {
                    frame.left = m_Data[Index * sizeof(T)];
                    frame.right = m_Data[Index * sizeof(T) + 1];
                    return frame;
                }
            }
        }
    private:
        AudioFormat m_Format;
        uint8_t* m_Data = nullptr;
        size_t m_Size = 0;
    };
}
