#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

class AudioUtils
{
public:
    struct AudioData
    {
        std::vector<float> samples;
        int sampleRate;
        [[nodiscard]] size_t getNumSamples() const
        {
            return samples.size();
        }
    };

    AudioUtils();

    static AudioData readWav(const std::string &filePath);

private:
    inline static const std::string DEFAULT_PATH = "./data/";
};