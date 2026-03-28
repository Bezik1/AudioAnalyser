#include "AudioUtils.hpp"

AudioUtils::AudioUtils() {}

AudioUtils::AudioData AudioUtils::readWav(std::string_view filePath)
{
    std::filesystem::path fullPath = std::filesystem::path(DEFAULT_PATH) / filePath;
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
                    data.data.samples[i] = (sample / 32768.0f);
                }
            }
            else if (data.fmt.bitsPerSample == 8)
            {
                for (int i = 0; i < numSamples; ++i)
                {
                    uint8_t sample;
                    file.read(reinterpret_cast<char *>(&sample), sizeof(uint8_t));
                    data.data.samples[i] = ((sample - 128.0f) / 128.0f);
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