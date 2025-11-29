#pragma once
#include "AudioNode.h"
#include "Containers/BufferView.h"

typedef struct kiss_fftr_state* kiss_fftr_cfg;

namespace Nova
{
    struct FFTComplex { float i, r; };

    enum class FFTWindow
    {
        Rectangular,
        Hann,
        Hamming,
        Blackman,
        BlackmanHarris,
        Triangle,
    };

    float HannWindow(uint32_t n, uint32_t fftSize);
    float HammingWindow(uint32_t n, uint32_t fftSize);;
    float RectangularWindow(uint32_t n, uint32_t fftSize);
    float BlackmanWindow(uint32_t n, uint32_t fftSize);
    float BlackmanHarrisWindow(uint32_t n, uint32_t fftSize);
    float TriangleWindow(uint32_t n, uint32_t fftSize);

    Function<float(uint32_t, uint32_t)> GetFFTWindow(FFTWindow window);
    float ApplyWindow(FFTWindow window, uint32_t n, uint32_t fftSize);

    class FFTAudioNode final : public AudioNode
    {
    public:
        static constexpr uint32_t MaxFFTSize = 2048;
        bool Initialize(AudioSystem* system) override;
        void Destroy() override;
        void OnProcess(const float** inFrames, uint32_t inFrameCount, float** outFrames,uint32_t outFrameCount) override;
        void SetFFTSize(uint32_t fftSize);
        void SetFFTWindow(FFTWindow window);

        BufferView<float> GetFrequencies();
        uint32_t GetInputBusCount() const override;
        uint32_t GetOutputBusCount() const override;
        uint32_t GetNodeFlags() const override;
    private:
        uint32_t m_FFTSize = MaxFFTSize;
        FFTWindow m_FFTWindow = FFTWindow::Rectangular;
        kiss_fftr_cfg m_Config = nullptr;
        float m_InputBuffer[MaxFFTSize]{};
        FFTComplex m_OutputBuffer[MaxFFTSize / 2 + 1]{};
        float m_Magnitudes[MaxFFTSize / 2 + 1]{};
    };
}