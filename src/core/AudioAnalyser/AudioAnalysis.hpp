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
    // prywatne pola lub metody
};