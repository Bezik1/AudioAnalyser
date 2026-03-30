#pragma once
#include <vector>
#include <cmath>
#include <algorithm>

class AudioAnalyser
{
public:
    /**
     * @brief 
     * 
     */
    struct FrequencyData
    {
        float amplitude;
        float phase;
        float frequency;
    };

    AudioAnalyser();

    /**
     * @brief
     *
     * @details
     *
     * Why numFrequencies = samples.size() / 2 + 1?
     * The explanation is the fact that numFrequencies gives us number of necessary frequencies
     * to capture the whole complex sound we need exactly num_sample_freq / 2 + 1. This fact comes
     * from Nyquist-Shannon sampling theorem.
     *
     * @param samples
     * @param sampleRate
     * @return std::vector<FrequencyData>
     */
    static std::vector<FrequencyData>
    discreteFourierTransform(const std::vector<float> &samples, int sampleRate);

    /**
     * @brief 
     * 
     * @param spectrum 
     * @param numSamples 
     * @param sampleRate 
     * @return std::vector<float> 
     */
    static std::vector<float>
    reconstruct(const std::vector<FrequencyData> &spectrum, int numSamples, int sampleRate);

private:
};