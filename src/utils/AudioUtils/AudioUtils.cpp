#include "AudioUtils.hpp"

AudioUtils::AudioUtils() {}

AudioUtils::AudioData AudioUtils::readWav(std::string_view filePath)
{
    std::filesystem::path fullPath(filePath);
    std::ifstream file(fullPath, std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("Couldn't open the file with the given path.");

    AudioUtils::AudioData data;

    std::string chunkID(CHUNK_ID_SIZE, '\0');
    file.read(chunkID.data(), CHUNK_ID_SIZE);

    if (file.gcount() != static_cast<std::streamsize>(CHUNK_ID_SIZE) || chunkID != CHUNK_ID)
        throw std::runtime_error("Couldn't read the chunk identifier.");
    data.chunkID = chunkID;

    int chunkSize;
    file.read(reinterpret_cast<char *>(&chunkSize), CHUNK_SIZE_SIZE);

    if (file.gcount() != CHUNK_SIZE_SIZE)
        throw std::runtime_error("Couldn't read chunk size.");
    data.chunkSize = chunkSize;

    std::string format(FORMAT_SIZE, '\0');
    file.read(format.data(), FORMAT_SIZE);

    if (file.gcount() != static_cast<std::streamsize>(FORMAT_SIZE) || format != FORMAT)
        throw std::runtime_error("Couldn't read format.");
    data.format = format;

    while (file)
    {
        std::string subchunkID(4, '\0');
        file.read(subchunkID.data(), CHUNK_ID_SIZE);

        if (file.gcount() == 0)
            break;

        if (file.gcount() < CHUNK_ID_SIZE)
            throw std::runtime_error("Couldn't read subchunk identifier.");

        int subchunkSize;
        file.read(reinterpret_cast<char *>(&subchunkSize), CHUNK_SIZE_SIZE);

        if (subchunkID == SUB_CHUNK_1_ID)
        {
            data.fmt.chunkID = subchunkID;
            data.fmt.chunkSize = subchunkSize;

            file.read(reinterpret_cast<char *>(&data.fmt.audioFormat), AUDIO_FORMAT_SIZE);
            file.read(reinterpret_cast<char *>(&data.fmt.numChannels), NUM_CHANNELS_SIZE);
            file.read(reinterpret_cast<char *>(&data.fmt.sampleRate), SAMPLE_RATE_SIZE);
            file.read(reinterpret_cast<char *>(&data.fmt.byteRate), BYTE_RATE_SIZE);
            file.read(reinterpret_cast<char *>(&data.fmt.blockAlign), BLOCK_ALIGN_SIZE);
            file.read(reinterpret_cast<char *>(&data.fmt.bitsPerSample), BITS_PER_SAMPLE_SIZE);

            if (subchunkSize > 16)
                file.seekg(subchunkSize - 16, std::ios::cur);
        }
        else if (subchunkID == SUB_CHUNK_2_ID)
        {
            data.data.chunkID = subchunkID;
            data.data.chunkSize = subchunkSize;

            int numSamples = 8 * data.data.chunkSize / (data.fmt.numChannels * data.fmt.bitsPerSample);
            data.data.samples.clear();
            data.data.samples.resize(numSamples);

            if (data.fmt.bitsPerSample == 16)
            {
                for (int i = 0; i < numSamples; ++i)
                {
                    int16_t sample;
                    file.read(reinterpret_cast<char *>(&sample), sizeof(int16_t));
                    data.data.samples[i] = (sample / 32768.0);
                }
            }
            else if (data.fmt.bitsPerSample == 8)
            {
                for (int i = 0; i < numSamples; ++i)
                {
                    uint8_t sample;
                    file.read(reinterpret_cast<char *>(&sample), sizeof(uint8_t));
                    data.data.samples[i] = ((sample - 128.0) / 128.0);
                }
            }
            else
                throw std::runtime_error("Unsupported bits per sample value.");
        }
        else
        {
            file.seekg(subchunkSize, std::ios::cur);
        }
    }

    return data;
}

AudioUtils::AudioData
AudioUtils::prepareSamplesToBeSaved(const std::vector<double> &samples,
                                    uint16_t numChannels,
                                    uint32_t sampleRate,
                                    uint16_t bitsPerSample)
{
    AudioData data;

    data.chunkID = CHUNK_ID;
    data.format = FORMAT;

    data.fmt.chunkID = SUB_CHUNK_1_ID;
    data.fmt.chunkSize = 16;
    data.fmt.audioFormat = 1;
    data.fmt.numChannels = numChannels;
    data.fmt.sampleRate = sampleRate;
    data.fmt.bitsPerSample = bitsPerSample;
    data.fmt.byteRate = sampleRate * numChannels * bitsPerSample / 8;
    data.fmt.blockAlign = numChannels * bitsPerSample / 8;

    data.data.chunkID = SUB_CHUNK_2_ID;
    data.data.samples = samples;
    data.data.chunkSize = samples.size() * (bitsPerSample / 8);

    data.chunkSize = 36 + data.data.chunkSize;

    return data;
}

void AudioUtils::saveWav(const AudioUtils::AudioData &audioData, std::string_view filePath)
{
    std::filesystem::path fullPath(filePath);
    std::ofstream file(fullPath, std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("Couldn't open the file for writing: " + fullPath.string());

    file.write(audioData.chunkID.data(), CHUNK_ID_SIZE);
    file.write(reinterpret_cast<const char *>(&audioData.chunkSize), CHUNK_SIZE_SIZE);
    file.write(audioData.format.data(), FORMAT_SIZE);

    file.write(audioData.fmt.chunkID.data(), CHUNK_ID_SIZE);
    file.write(reinterpret_cast<const char *>(&audioData.fmt.chunkSize), CHUNK_SIZE_SIZE);
    file.write(reinterpret_cast<const char *>(&audioData.fmt.audioFormat), AUDIO_FORMAT_SIZE);
    file.write(reinterpret_cast<const char *>(&audioData.fmt.numChannels), NUM_CHANNELS_SIZE);
    file.write(reinterpret_cast<const char *>(&audioData.fmt.sampleRate), SAMPLE_RATE_SIZE);
    file.write(reinterpret_cast<const char *>(&audioData.fmt.byteRate), BYTE_RATE_SIZE);
    file.write(reinterpret_cast<const char *>(&audioData.fmt.blockAlign), BLOCK_ALIGN_SIZE);
    file.write(reinterpret_cast<const char *>(&audioData.fmt.bitsPerSample), BITS_PER_SAMPLE_SIZE);

    file.write(audioData.data.chunkID.data(), CHUNK_ID_SIZE);
    file.write(reinterpret_cast<const char *>(&audioData.data.chunkSize), CHUNK_SIZE_SIZE);

    if (audioData.fmt.bitsPerSample == 16)
    {
        for (int i = 0; i < audioData.data.samples.size(); i++)
        {
            double clamped = std::clamp(audioData.data.samples.at(i), -1.0, 1.0);
            int16_t clampedSample = static_cast<int16_t>(clamped * 32767.0);

            file.write(reinterpret_cast<const char *>(&clampedSample), sizeof(int16_t));
            if (!file)
                throw std::runtime_error("Failed writing 16-bit sample to file: " + fullPath.string());
        }
    }
    else if (audioData.fmt.bitsPerSample == 8)
    {
        for (int i = 0; i < audioData.data.samples.size(); i++)
        {
            double clamped = std::clamp(audioData.data.samples.at(i), -1.0, 1.0);
            uint8_t clampedSample = static_cast<uint8_t>((clamped + 1.0) * 127.5f);

            file.write(reinterpret_cast<const char *>(&clampedSample), sizeof(uint8_t));
            if (!file)
                throw std::runtime_error("Failed writing 8-bit sample to file: " + fullPath.string());
        }
    }
    else
    {
        throw std::runtime_error("Unsupported bits per sample format.");
    }
}