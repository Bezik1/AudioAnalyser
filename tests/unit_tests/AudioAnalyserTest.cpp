#include <gtest/gtest.h>
#include <vector>
#include <cmath>

#include "../../src/core/AudioAnalyser/AudioAnalyser.hpp"

/**
 * @brief Tests for AudioAnalyser class.
 * 
 */
class AudioAnalyserTest : public testing::Test 
{
protected:
    static constexpr double FREQUENCY_ERROR = 1e-5;
    static constexpr double AMPLITUDE_ERROR = 1e-3f;
    static constexpr double RECONSTRUCTION_ERROR = 1e-5f;
};

/**
 * @brief Compare the frequency data obtained from discreteFourierTransform function
 * and the real amplitudes and frequencies, that made up the signal.
 *
 */
TEST_F(AudioAnalyserTest, DFT_TEST)
{
    int numSamples = 10000;
    int sampleRate = 20;

    std::vector<double> amplitudes = {0.02, 0.005, 0.01, 0.005, 0.09, 0.5f};
    std::vector<double> phases = {3.0, 2.5, 0.01, -1.12, 0.5, 0.1f};
    std::vector<double> samples(numSamples, 0.0);
    int numFreq = amplitudes.size();

    for (int i = 0; i < numFreq; i++)
    {
        for (int t = 0; t < numSamples; t++)
        {
            double time = static_cast<double>(t) / sampleRate;
            samples[t] += amplitudes[i] * std::cos(2.0 * M_PI * i * time + phases[i]);
        }
    }

    std::vector<AudioAnalyser::FrequencyData> spectrum =
        AudioAnalyser::discreteFourierTransform(samples, sampleRate);

    std::sort(spectrum.begin(), spectrum.end(), [](const auto &a, const auto &b)
              { return a.amplitude > b.amplitude; });

    std::sort(spectrum.begin(), spectrum.begin() + numFreq,
              [](const auto &a, const auto &b)
              { return a.frequency < b.frequency; });

    for (int i = 0; i < numFreq; i++)
    {
        auto freq = spectrum[i];

        EXPECT_NEAR(freq.frequency, i, FREQUENCY_ERROR);
        EXPECT_NEAR(freq.amplitude, amplitudes[i], AMPLITUDE_ERROR);
    }
}

/**
 * @brief Compare the frequency data obtained from fastFourierTransform function
 * and the real amplitudes and frequencies, that made up the signal.
 */
TEST_F(AudioAnalyserTest, FFT_TEST)
{
    int numSamples = 8192;
    int sampleRate = numSamples;

    std::vector<double> amplitudes = {0.02, 0.005, 0.01, 0.005, 0.09, 0.5f};
    std::vector<double> phases = {3.0, 2.5, 0.01, -1.12, 0.5, 0.1f};
    std::vector<double> samples(numSamples, 0.0);
    int numFreq = amplitudes.size();

    for (int i = 0; i < numFreq; i++)
    {
        for (int t = 0; t < numSamples; t++)
        {
            double time = static_cast<double>(t) / sampleRate;
            samples[t] += amplitudes[i] * std::cos(2.0 * M_PI * i * time + phases[i]);
        }
    }

    std::vector<AudioAnalyser::FrequencyData> spectrum =
        AudioAnalyser::fastFourierTransform(samples, sampleRate);

    std::sort(spectrum.begin(), spectrum.end(), [](const auto &a, const auto &b)
              { return a.amplitude > b.amplitude; });

    std::sort(spectrum.begin(), spectrum.begin() + numFreq,
              [](const auto &a, const auto &b)
              { return a.frequency < b.frequency; });

    for (int i = 0; i < numFreq; i++)
    {
        auto freq = spectrum[i];

        EXPECT_NEAR(freq.frequency, i, FREQUENCY_ERROR);
        EXPECT_NEAR(freq.amplitude, amplitudes[i], AMPLITUDE_ERROR);
    }
}

/**
 * @brief Check's whether the reconstruction process of DFT was succesfull.
 * 
 */
TEST_F(AudioAnalyserTest, RECONSTRUCT_DFT_TEST)
{
    int numSamples = 10000;
    int sampleRate = 20;

    std::vector<double> amplitudes = {0.02, 0.005, 0.01, 0.005, 0.09, 0.5f};
    std::vector<double> phases = {3.0, 2.5, 0.01, -1.12, 0.5, 0.1f};
    std::vector<double> samples(numSamples, 0.0);
    int numFreq = amplitudes.size();

    for (int i = 0; i < numFreq; i++)
    {
        for (int t = 0; t < numSamples; t++)
        {
            double time = static_cast<double>(t) / sampleRate;
            samples[t] += amplitudes[i] * std::cos(2.0 * M_PI * i * time + phases[i]);
        }
    }

    std::vector<AudioAnalyser::FrequencyData> spectrum =
        AudioAnalyser::discreteFourierTransform(samples, sampleRate);

    std::vector<double> reconstructedSamples =
        AudioAnalyser::reconstructDFT(spectrum, samples.size(), sampleRate);

    for (int i = 0; i < reconstructedSamples.size(); i++)
    {
        EXPECT_NEAR(reconstructedSamples[i], samples[i], RECONSTRUCTION_ERROR);
    }
}