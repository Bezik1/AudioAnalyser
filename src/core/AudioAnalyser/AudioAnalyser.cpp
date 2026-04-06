#include "AudioAnalyser.hpp"
#include <cmath>
#include <algorithm>

AudioAnalyser::AudioAnalyser() {}

std::vector<AudioAnalyser::FrequencyData>
AudioAnalyser::discreteFourierTransform(const std::vector<float> &samples, int sampleRate)
{
    size_t numSamples = samples.size();
    int numFrequencies = numSamples / 2 + 1;
    std::vector<FrequencyData> spectrum(numFrequencies);
    float frequencyStepSize = static_cast<float>(sampleRate) / numSamples;

    for (int i = 0; i < numFrequencies; i++)
    {
        std::complex<float> sum(0, 0);
        for (int j = 0; j < numSamples; j++)
        {
            float angle = -2.0f * M_PI * i * j / numSamples;
            std::complex<float> exponent(cos(angle), sin(angle));
            sum += samples[j] * exponent;
        }

        std::complex<float> avg = sum / static_cast<float>(numSamples);
        spectrum[i].amplitude = (i == 0) ? std::abs(avg) : 2.0f * std::abs(avg);
        spectrum[i].phase = std::arg(avg);
        spectrum[i].frequency = static_cast<float>(i) * frequencyStepSize;
    }
    return spectrum;
}

std::vector<float> AudioAnalyser::reconstructDFT(const std::vector<FrequencyData> &spectrum, int numSamples, int sampleRate)
{
    std::vector<float> signal(numSamples, 0.0f);

    for (size_t i = 0; i < numSamples; i++)
    {
        float currentSample = 0.0f;
        for (const auto &freq : spectrum)
        {
            float t = static_cast<float>(i) / sampleRate;
            currentSample += freq.amplitude * cos(2.0f * M_PI * freq.frequency * t + freq.phase);
        }
        signal[i] = currentSample;
    }

    return signal;
}

std::vector<std::complex<float>>
AudioAnalyser::fftRecursive(std::vector<std::complex<float>> &polynomial, bool invert)
{
    int numSamples = polynomial.size();
    if (numSamples <= 1)
        return polynomial;

    std::vector<std::complex<float>> even(numSamples / 2);
    std::vector<std::complex<float>> odd(numSamples / 2);
    for (int i = 0; i < numSamples / 2; i++)
    {
        even[i] = polynomial[2 * i];
        odd[i] = polynomial[2 * i + 1];
    }

    std::vector<std::complex<float>> y_even = fftRecursive(even, invert);
    std::vector<std::complex<float>> y_odd = fftRecursive(odd, invert);

    std::vector<std::complex<float>> y(numSamples);

    float angle = (invert ? 2.0f * M_PI : -2.0f * M_PI) / numSamples;
    std::complex<float> nthExponent(std::cos(angle), std::sin(angle));
    std::complex<float> exponent(1, 0);

    for (int j = 0; j < numSamples / 2; j++)
    {
        std::complex<float> common = exponent * y_odd[j];
        y[j] = y_even[j] + common;
        y[j + numSamples / 2] = y_even[j] - common;
        exponent *= nthExponent;
    }
    return y;
}

std::vector<AudioAnalyser::FrequencyData>
AudioAnalyser::fastFourierTransform(const std::vector<float> &samples, int sampleRate)
{
    int originalSize = samples.size();
    int n = 1;
    while (n < originalSize)
        n *= 2;

    std::vector<std::complex<float>> complexSamples(n, {0, 0});
    for (int i = 0; i < originalSize; i++)
        complexSamples[i] = {samples[i], 0};

    std::vector<std::complex<float>> result = fftRecursive(complexSamples, false);

    int numFrequencies = n / 2 + 1;
    std::vector<FrequencyData> spectrum(numFrequencies);
    float frequencyStep = static_cast<float>(sampleRate) / n;

    for (int i = 0; i < numFrequencies; i++)
    {
        float amplitude = (i == 0 || i == n / 2) ? std::abs(result[i]) : 2.0f * std::abs(result[i]);
        spectrum[i].amplitude = amplitude / n;
        spectrum[i].phase = std::arg(result[i]);
        spectrum[i].frequency = i * frequencyStep;
    }

    return spectrum;
}

std::vector<float>
AudioAnalyser::reconstructFFT(const std::vector<FrequencyData> &spectrum, int numSamples, int sampleRate)
{
    int n = 1;
    while (n < numSamples)
        n *= 2;

    std::vector<std::complex<float>> complexSpectrum(n, {0, 0});
    float frequencyStep = static_cast<float>(sampleRate) / n;

    for (const auto &freq : spectrum)
    {
        int i = std::round(freq.frequency / frequencyStep);

        if (i < 0 || i > n / 2)
            continue;

        float normAmp = (i == 0 || i == n / 2) ? freq.amplitude : freq.amplitude / 2.0f;
        normAmp *= n;

        float real = normAmp * std::cos(freq.phase);
        float imag = normAmp * std::sin(freq.phase);

        complexSpectrum[i] = {real, imag};

        if (i > 0 && i < n / 2)
            complexSpectrum[n - i] = std::conj(complexSpectrum[i]);
    }

    std::vector<std::complex<float>> reconstructedComplex = fftRecursive(complexSpectrum, true);

    std::vector<float> signal(numSamples);
    for (int i = 0; i < numSamples; i++)
    {
        signal[i] = reconstructedComplex[i].real() / static_cast<float>(n);
    }

    return signal;
}