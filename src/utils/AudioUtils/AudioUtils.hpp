#pragma once

#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <cmath>

/**
 * @brief Utils for reading and managing .wav audio files.
 *
 * @details Implementation uses following <a href="http://soundfile.sapp.org/doc/WaveFormat/">Specification</a>.
 * It is focused on bit by bit analysis of .wav audio file. It maps the collected informations in AudioData struct.
 *
 */
class AudioUtils
{
public:
    /**
     * @brief Basic building block of all data written in .wav files.
     *
     * @param chunkID It contains the name of specific chunk, from followign set: {"RIFF", "fmt ", "data"}
     * @param chunkSize Size of the current chunk.
     *
     * @note chunkID "fmt " field has a space because it could also be another type of data message, but
     * we need to reserve space for it.
     */
    struct SubChunk
    {
        std::string chunkID;
        uint32_t chunkSize;
    };

    /**
     * @brief FormatSubChunk describes the sound data's format.
     *
     * @param audioFormat Pulse Code Modulation (PCM) = 1. Program does not consider values other than 1
     * because they indicate some sort of compression.
     * @param numChannels Describes number of samples for each frame. Mono = 1, Stereo = 2.
     * @param sampleRate Describes how many samples were saved druing one second. Values: {44100, 4800, 9600}.
     * @param byteRate sampleRate * numChannels * bitsPerSample / 8
     * @param blockAlign numChannels * bitsPerSample / 8
     * @param bitsPerSample Number of bits assigned to each sample. Values: {8, 16}.
     *
     * @note This struct uses fixed-width integer types because program is sure about the amount of bits
     * being part of specific variables in the .wav file, however normal types like `int` or `short` do
     * not have guarantee of being for example 4 bits long. Having that in mind the only valid implementation
     * for this problem is to specify number of bits in each variable type.
     */
    struct FormatSubChunk : SubChunk
    {
        uint16_t audioFormat;
        uint16_t numChannels;
        uint32_t sampleRate;
        uint32_t byteRate;
        uint16_t blockAlign;
        uint16_t bitsPerSample;
    };

    /**
     * @brief DataSubChunk contains the size of data and the actual sound data.
     *
     * @param samples Vector of samples, of following normalized format: -1.0 <= sample <= 1.0.
     */
    struct DataSubChunk : SubChunk
    {
        std::vector<double> samples;
    };

    /**
     * @brief Contains informations about sound from .wav file.
     *
     * @details The canonical WAVE format starts with the RIFF header:
     * - The "RIFF header" describes size of the file and contains "RIFF" identifier.
     * - The "WAVE" format consists of two subchunks: "fmt " and "data":
     *
     * @param format Contains the letters "WAVE".
     * @param fmt Contains format data, describing sound file.
     * @param data Contains actual sound file data.
     *
     */
    struct AudioData : SubChunk
    {
        std::string format;

        FormatSubChunk fmt;
        DataSubChunk data;

        [[nodiscard]] size_t
        getNumSamples() const
        {
            return data.samples.size();
        }
    };

    AudioUtils();

    /**
     * @brief Reads .wav file and returns AudioData struct containing actual sound data and it's specification.
     *
     * @details This implementation sticks to RAII system. To do so it uses std::ifstream class, which
     * guarantee that file will close no further, than before last block of the sequence.
     *
     * Class can throw various runtime errors, depending on whether some obligatory field is corrupted.
     *
     * This method was created, according to .wav file specification described in ./docs/AUDIO_FILES.md.
     *
     * Calculations for numSamples:
     * ```
     * subChunk2Size = numSamples * numChannels * bitsPerSample / 8
     * numSamples = 8 * subChunk2Size / (numChannels * bitsPerSample)
     * ```
     *
     * @param filePath path to the file in the default `./data` directory.
     *
     * @return AudioData
     */
    static AudioData readWav(std::string_view filePath);

    /**
     * @brief Prepares samples to be saved in a .wav file.
     *
     * @details It uses the informations about numChannels, sampleRate and bitsPersSample
     * to calculate other properties of audio file and returns AudioData object.
     *
     * @param samples samples in the form of vector of doubles.
     * @param numChannels informs about number of dominating voices mono / stereo.
     * @param sampleRate rate at which samples were measured in bits per second.
     * @param bitsPerSample it is the value of bits per sample.
     *
     * @return AudioData Ready to save audio object.
     */
    static AudioData prepareSamplesToBeSaved(const std::vector<double> &samples,
                                          uint16_t numChannels,
                                          uint32_t sampleRate,
                                          uint16_t bitsPerSample);

    /**
     * @brief Saves the .wav file into given directory.
     *
     * @details This implementation sticks to RAII system. To do so it uses std::ofsteam class, which
     * guarantee that file will close no further, than before last block of the sequence.
     *
     * This method was created, according to .wav file specification described in ./docs/AUDIO_FILES.md.
     *
     * @param audioData data to be saved in AudioData format.
     * @param filePath path to the file in the default `./data` directory.
     */
    static void saveWav(const AudioUtils::AudioData &audioData, std::string_view filePath);

private:
    inline static const std::string CHUNK_ID = "RIFF";
    inline static const std::string FORMAT = "WAVE";
    inline static const std::string SUB_CHUNK_1_ID = "fmt ";
    inline static const std::string SUB_CHUNK_2_ID = "data";

    static constexpr int CHUNK_ID_SIZE = 4;
    static constexpr int CHUNK_SIZE_SIZE = 4;
    static constexpr int FORMAT_SIZE = 4;

    static constexpr int AUDIO_FORMAT_SIZE = 2;
    static constexpr int NUM_CHANNELS_SIZE = 2;
    static constexpr int SAMPLE_RATE_SIZE = 4;
    static constexpr int BYTE_RATE_SIZE = 4;
    static constexpr int BLOCK_ALIGN_SIZE = 2;
    static constexpr int BITS_PER_SAMPLE_SIZE = 2;
};