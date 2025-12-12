#include "FFTAudioNode.h"
#include <kiss_fftr.h>

namespace Nova
{
    float HannWindow(const uint32_t n, const uint32_t fftSize)
    {
        return 0.5f * (1.0f - Math::Cos(Math::Tau * n / (fftSize - 1)));
    }

    float HammingWindow(const uint32_t n, const uint32_t fftSize)
    {
        return 0.54f - 0.46f * Math::Cos(Math::Tau * n / (fftSize - 1));
    }

    float RectangularWindow(const uint32_t n, const uint32_t fftSize)
    {
        (void)n;
        (void)fftSize;
        return 1.0f;
    }

    float BlackmanWindow(const uint32_t n, const uint32_t fftSize)
    {
        constexpr float a0 = 0.42f;
        constexpr float a1 = 0.5f;
        constexpr float a2 = 0.08f;

        const float t = Math::Tau * n / (fftSize - 1);
        return a0 - a1 * Math::Cos(t) + a2 * Math::Cos(2.0f * t);
    }

    float BlackmanHarrisWindow(const uint32_t n, const uint32_t fftSize)
    {
        constexpr float a0 = 0.35875f;
        constexpr float a1 = 0.48829f;
        constexpr float a2 = 0.14128f;
        constexpr float a3 = 0.01168f;

        const float t = Math::Tau * n / (fftSize - 1);
        return a0 - a1 * Math::Cos(t) + a2 * Math::Cos(2.0f * t) - a3 * Math::Cos(3.0f * t);
    }

    float TriangleWindow(const uint32_t n, const uint32_t fftSize)
    {
        const float half = 0.5f * (fftSize - 1);
        return 1.0f - Math::Abs((n - half) / half);
    }

    Function<float(uint32_t, uint32_t)> GetFFTWindow(const FFTWindow window)
    {
        switch (window)
        {
        case FFTWindow::Rectangular: return RectangularWindow;
        case FFTWindow::Hann: return HannWindow;
        case FFTWindow::Hamming: return HammingWindow;
        case FFTWindow::Blackman: return BlackmanWindow;
        case FFTWindow::BlackmanHarris: return BlackmanHarrisWindow;
        case FFTWindow::Triangle: return TriangleWindow;
        }

        return RectangularWindow;
    }

    float ApplyWindow(const FFTWindow window, const uint32_t n, const uint32_t fftSize)
    {
        return GetFFTWindow(window)(n, fftSize);
    }

    bool FFTAudioNode::Initialize(AudioSystem* system)
    {
        if (!AudioNode::Initialize(system)) return false;
        std::free(m_Config);
        m_Config = kiss_fftr_alloc(m_FFTSize, 0, nullptr, nullptr);
        if (!m_Config) return false;
        return true;
    }

    void FFTAudioNode::Destroy()
    {
        AudioNode::Destroy();
        std::free(m_Config);
    }

    void FFTAudioNode::OnProcess(const float** inFrames, const uint32_t inFrameCount, float** outFrames, const uint32_t outFrameCount)
    {
        for (int sampleIndex = 0; sampleIndex < m_FFTSize && sampleIndex < inFrameCount; sampleIndex++)
        {
            m_InputBuffer[sampleIndex] = inFrames[0][sampleIndex] * ApplyWindow(m_FFTWindow, sampleIndex, m_FFTSize);
        }

        kiss_fftr(m_Config, m_InputBuffer, (kiss_fft_cpx*)m_OutputBuffer);

        for (int sampleIndex = 0; sampleIndex < m_FFTSize && sampleIndex < inFrameCount; sampleIndex++)
            m_Magnitudes[sampleIndex] = Math::Sqrt(m_OutputBuffer[sampleIndex].r * m_OutputBuffer[sampleIndex].r + m_OutputBuffer[sampleIndex].i * m_OutputBuffer[sampleIndex].i);

    }

    uint32_t FFTAudioNode::GetFFTSize() const
    {
        return m_FFTSize;
    }

    void FFTAudioNode::SetFFTSize(const uint32_t fftSize)
    {
        NOVA_ASSERT((fftSize & (fftSize - 1)) == 0, "FFTSize must be a power of 2!");
        m_FFTSize = fftSize;
        std::free(m_Config);
        m_Config = kiss_fftr_alloc(m_FFTSize, 0, nullptr, nullptr);
    }

    void FFTAudioNode::SetFFTWindow(const FFTWindow window)
    {
        m_FFTWindow = window;
    }

    FFTWindow FFTAudioNode::GetFFTWindow() const
    {
        return m_FFTWindow;
    }

    BufferView<float> FFTAudioNode::GetFrequencies()
    {
        return { m_Magnitudes, m_FFTSize / 2 };
    }

    uint32_t FFTAudioNode::GetInputBusCount() const
    {
        return 1;
    }

    uint32_t FFTAudioNode::GetOutputBusCount() const
    {
        return 1;
    }

    uint32_t FFTAudioNode::GetNodeFlags() const
    {
        return MA_NODE_FLAG_PASSTHROUGH;
    }
}
