#include "AudioAnalyser.hpp"
#include <cmath>
#include <algorithm>

AudioAnalyser::AudioAnalyser() {}

std::vector<AudioAnalyser::FrequencyData>
AudioAnalyser::discreteFourierTransform(const std::vector<double> &samples, int sampleRate)
{
    size_t numSamples = samples.size();
    int numFrequencies = numSamples / 2 + 1;
    std::vector<FrequencyData> spectrum(numFrequencies);
    double frequencyStepSize = static_cast<double>(sampleRate) / numSamples;

    for (int i = 0; i < numFrequencies; i++)
    {
        std::complex<double> sum(0, 0);
        for (int j = 0; j < numSamples; j++)
        {
            double angle = -2.0 * M_PI * i * j / numSamples;
            std::complex<double> exponent(cos(angle), sin(angle));
            sum += samples[j] * exponent;
        }

        std::complex<double> avg = sum / static_cast<double>(numSamples);
        spectrum[i].amplitude = (i == 0 || i == numFrequencies) ? std::abs(avg) : 2.0 * std::abs(avg);
        spectrum[i].phase = std::arg(avg);
        spectrum[i].frequency = static_cast<double>(i) * frequencyStepSize;
    }
    return spectrum;
}

std::vector<double> AudioAnalyser::reconstructDFT(const std::vector<FrequencyData> &spectrum, int numSamples, int sampleRate)
{
    std::vector<double> signal(numSamples, 0.0);

    for (size_t i = 0; i < numSamples; i++)
    {
        double currentSample = 0.0;
        for (const auto &freq : spectrum)
        {
            double t = static_cast<double>(i) / sampleRate;
            currentSample += freq.amplitude * cos(2.0 * M_PI * freq.frequency * t + freq.phase);
        }
        signal[i] = currentSample;
    }

    return signal;
}

std::vector<std::complex<double>>
AudioAnalyser::fftRecursive(std::vector<std::complex<double>> &polynomial, bool invert)
{
    int numSamples = polynomial.size();
    if (numSamples <= 1)
        return polynomial;

    std::vector<std::complex<double>> even(numSamples / 2);
    std::vector<std::complex<double>> odd(numSamples / 2);
    for (int i = 0; i < numSamples / 2; i++)
    {
        even[i] = polynomial[2 * i];
        odd[i] = polynomial[2 * i + 1];
    }

    std::vector<std::complex<double>> yEven = fftRecursive(even, invert);
    std::vector<std::complex<double>> yOdd = fftRecursive(odd, invert);

    std::vector<std::complex<double>> y(numSamples);

    double angle = (invert ? 2.0 * M_PI : -2.0 * M_PI) / numSamples;
    std::complex<double> nthW(std::cos(angle), std::sin(angle));
    std::complex<double> w(1, 0);

    for (int j = 0; j < numSamples / 2; j++)
    {
        std::complex<double> common = w * yOdd[j];
        y[j] = yEven[j] + common;
        y[j + numSamples / 2] = yEven[j] - common;
        w *= nthW;
    }
    return y;
}

std::vector<AudioAnalyser::FrequencyData>
AudioAnalyser::fastFourierTransform(const std::vector<double> &samples, int sampleRate)
{
    int originalSize = samples.size();
    int n = 1;
    while (n < originalSize)
        n *= 2;

    std::vector<std::complex<double>> complexSamples(n, {0, 0});
    for (int i = 0; i < originalSize; i++)
        complexSamples[i] = {samples[i], 0};

    std::vector<std::complex<double>> result = fftRecursive(complexSamples, false);

    int numFrequencies = n / 2 + 1;
    std::vector<FrequencyData> spectrum(numFrequencies);
    double frequencyStep = static_cast<double>(sampleRate) / n;

    for (int i = 0; i < numFrequencies; i++)
    {
        double amplitude = (i == 0 || i == n / 2) ? std::abs(result[i]) : 2.0 * std::abs(result[i]);
        spectrum[i].amplitude = amplitude / n;
        spectrum[i].phase = std::arg(result[i]);
        spectrum[i].frequency = i * frequencyStep;
    }

    return spectrum;
}

std::vector<double>
AudioAnalyser::reconstructFFT(const std::vector<FrequencyData> &spectrum, int numSamples, int sampleRate)
{
    int n = 1;
    while (n < numSamples)
        n *= 2;

    std::vector<std::complex<double>> complexSpectrum(n, {0, 0});
    double frequencyStep = static_cast<double>(sampleRate) / n;

    for (const auto &freq : spectrum)
    {
        int i = std::round(freq.frequency / frequencyStep);

        if (i < 0 || i > n / 2)
            continue;

        double normAmp = (i == 0 || i == n / 2) ? freq.amplitude : freq.amplitude / 2.0;
        normAmp *= n;

        double real = normAmp * std::cos(freq.phase);
        double imag = normAmp * std::sin(freq.phase);

        complexSpectrum[i] = {real, imag};

        if (i > 0 && i < n / 2)
            complexSpectrum[n - i] = std::conj(complexSpectrum[i]);
    }

    std::vector<std::complex<double>> reconstructedComplex = fftRecursive(complexSpectrum, true);

    std::vector<double> signal(numSamples);
    for (int i = 0; i < numSamples; i++)
        signal[i] = reconstructedComplex[i].real() / static_cast<double>(n);

    return signal;
}