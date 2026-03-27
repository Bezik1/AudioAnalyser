#include "AudioUtils.hpp"

AudioUtils::AudioUtils() {}

auto AudioUtils::readWav(const std::string &filePath)
{
    std::string fullPath = AudioUtils::AudioData + filePath;
    std::ifstream file(fullPath, std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("Couldn't open the file, with the given path.");
}