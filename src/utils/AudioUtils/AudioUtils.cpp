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
    file.read(reinterpret_cast<char *>(&chunkSize), CHUNK_SIZE);

    if (file.gcount() != CHUNK_SIZE)
        throw std::runtime_error("Couldn't read chunk size.");
    data.chunkSize = chunkSize;

    std::string format(FORMAT_SIZE, '\0');
    file.read(format.data(), FORMAT_SIZE);

    if (file.gcount() != static_cast<std::streamsize>(FORMAT_SIZE) || format != FORMAT)
        throw std::runtime_error("Couldn't read format.");
    data.format = format;

    return data;
}