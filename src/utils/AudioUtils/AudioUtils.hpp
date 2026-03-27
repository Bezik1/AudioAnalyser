#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

/**
 * @brief
 *
 */
class AudioUtils
{
public:
    /**
     * @brief
     *
     */
    struct AudioData
    {
        std::string chunkID;
        int chunkSize;
        std::string format;

        std::vector<float> samples;
        int sampleRate;

        [[nodiscard]] size_t getNumSamples() const
        {
            return samples.size();
        }
    };

    AudioUtils();

    /**
     * @brief
     *
     * @param filePath
     * @return AudioData
     */
    static AudioData readWav(std::string_view filePath);

private:
    inline static const std::string DEFAULT_PATH = "./data";

    inline static const std::string CHUNK_ID = "RIFF";
    inline static const std::string FORMAT = "WAVE";
    inline static const std::string SUB_CHUNK_1_ID = "fmt";
    inline static const std::string SUB_CHUNK_2_ID = "data";

    static constexpr int CHUNK_ID_SIZE = 4;
    static constexpr int CHUNK_SIZE = 4;
    static constexpr int FORMAT_SIZE = 4;
    static constexpr int SUB_CHUNK_1_SIZE = 16;
};