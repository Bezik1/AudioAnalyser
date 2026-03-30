#include "AudioAnalyser.hpp"

AudioAnalyser::AudioAnalyser() {}

std::vector<AudioAnalyser::FrequencyData>
AudioAnalyser::discreteFourierTransform(const std::vector<float> &samples, int sampleRate)
{
    int numFrequencies = samples.size() / 2 + 1;
    std::vector<FrequencyData> spectrum(numFrequencies);

    float frequencyStepSize = sampleRate / static_cast<float>(samples.size());

    for (int i = 0; i < numFrequencies; ++i)
    {
        float x = 0.0f;
        float y = 0.0f;

        for (int j = 0; j < samples.size(); ++j)
        {
            float angle = j / static_cast<float>(samples.size()) * 2 * M_PI * i;
            x += cos(angle) * samples[j];
            y += sin(angle) * samples[j];
        }

        float centerX = x / samples.size();
        float centerY = y / samples.size();

        spectrum[i].amplitude = 2.0f * sqrt(centerX * centerX + centerY * centerY);
        spectrum[i].phase = -atan2(centerY, centerX);
        spectrum[i].frequency = i * frequencyStepSize;
    }

    return spectrum;
}

std::vector<float>
AudioAnalyser::reconstruct(const std::vector<FrequencyData>& spectrum, int numSamples, int sampleRate)
{
    std::vector<float> signal(numSamples, 0.0f);

    for (size_t n = 0; n < numSamples; ++n)
    {
        float sample = 0.0f;
        for (size_t k = 0; k < spectrum.size(); ++k)
        {
            float amplitude = spectrum[k].amplitude;
            float phase = spectrum[k].phase;
            float freq = spectrum[k].frequency;

            float angle = 2.0f * M_PI * freq * n / sampleRate;
            sample += amplitude * cos(angle + phase);
        }
        signal[n] = sample;
    }

    return signal;
}