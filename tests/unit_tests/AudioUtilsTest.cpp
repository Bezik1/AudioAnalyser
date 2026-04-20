#include <gtest/gtest.h>

#include "../../src/utils/AudioUtils/AudioUtils.hpp"

/**
 * @brief Tests for AudioUtils class.
 * @test
 */
class AudioUtilsTest : public testing::Test
{
protected:
    inline static const std::string TEST_INPUT_PATH = "data/test/input_test.wav";
    inline static const std::string TEST_OUTPUT_PATH = "data/test/output_test.wav";
    inline static const std::string INCORRECT_TEST_PATH = "/invalid_path/incorrect_test.wav";

    static constexpr double FLOAT_PARSING_ERROR = 1.0 / 32767.0;

    AudioUtils audioUtils;
};

/**
 * @brief Test, whether program throws correct exception, if user
 * passed incorrect path to audioUtils.
 */
TEST_F(AudioUtilsTest, INPUT_FILE_NOT_FOUND_TEST)
{
    EXPECT_THROW({ audioUtils.readWav(INCORRECT_TEST_PATH); }, std::runtime_error);
}

/**
 * @brief Tests, if all fields in RIFF header are correctly stored.
 *
 */
TEST_F(AudioUtilsTest, WAV_CHUNK_DESCRIPTOR_TEST)
{
    auto data = audioUtils.readWav(TEST_INPUT_PATH);

    ASSERT_EQ(data.chunkID.size(), 4);
    ASSERT_EQ(data.chunkID, "RIFF");

    ASSERT_EQ(data.format.size(), 4);
    ASSERT_EQ(data.format, "WAVE");

    EXPECT_EQ(data.fmt.chunkID, "fmt ");
    EXPECT_EQ(data.fmt.audioFormat, 1);

    EXPECT_TRUE(data.fmt.numChannels == 1 || data.fmt.numChannels == 2);
    EXPECT_TRUE(data.fmt.bitsPerSample == 8 || data.fmt.bitsPerSample == 16);
    EXPECT_GT(data.fmt.sampleRate, 0);
}

/**
 * @brief Tests the format of data stored inside AudioData in data sub chunk.
 *
 */
TEST_F(AudioUtilsTest, WAV_DATA_FIELD_VERIFICATION_TEST)
{
    auto data = audioUtils.readWav(TEST_INPUT_PATH);

    EXPECT_EQ(data.data.chunkID, "data");
    EXPECT_GT(data.data.chunkSize, 0);

    int expectedSize = 8 * data.data.chunkSize / (data.fmt.numChannels * data.fmt.bitsPerSample);
    EXPECT_EQ(data.data.samples.size(), expectedSize);
    EXPECT_EQ(data.getNumSamples(), expectedSize);

    for (double sample : data.data.samples)
        ASSERT_TRUE(sample >= -1.0 && sample <= 1.0);
}

/**
 * @brief Test, whether program throws correct exception, if user
 * passed incorrect path to audioUtils.
 */
TEST_F(AudioUtilsTest, INCORRECT_OUTPUT_FILE_TEST)
{
    auto sampleData = audioUtils.readWav(TEST_INPUT_PATH);
    EXPECT_THROW({ audioUtils.saveWav(sampleData, INCORRECT_TEST_PATH); }, std::runtime_error);
}

/**
 * @brief Test, if data saved from data and dataAfterSave given by readWav after saving
 * the first file are the same.
 *
 */
TEST_F(AudioUtilsTest, WAV_DATA_SAVE_TEST)
{
    auto data = audioUtils.readWav(TEST_INPUT_PATH);
    audioUtils.saveWav(data, TEST_OUTPUT_PATH);

    auto dataAfterSave = audioUtils.readWav(TEST_OUTPUT_PATH);

    EXPECT_EQ(data.chunkID, dataAfterSave.chunkID);
    EXPECT_EQ(data.chunkSize, dataAfterSave.chunkSize);
    EXPECT_EQ(data.format, dataAfterSave.format);

    EXPECT_EQ(data.fmt.chunkID, dataAfterSave.fmt.chunkID);
    EXPECT_EQ(data.fmt.chunkSize, dataAfterSave.fmt.chunkSize);
    EXPECT_EQ(data.fmt.audioFormat, dataAfterSave.fmt.audioFormat);
    EXPECT_EQ(data.fmt.numChannels, dataAfterSave.fmt.numChannels);
    EXPECT_EQ(data.fmt.sampleRate, dataAfterSave.fmt.sampleRate);
    EXPECT_EQ(data.fmt.byteRate, dataAfterSave.fmt.byteRate);
    EXPECT_EQ(data.fmt.blockAlign, dataAfterSave.fmt.blockAlign);
    EXPECT_EQ(data.fmt.bitsPerSample, dataAfterSave.fmt.bitsPerSample);

    EXPECT_EQ(data.data.chunkID, dataAfterSave.data.chunkID);
    EXPECT_EQ(data.data.chunkSize, dataAfterSave.data.chunkSize);
    ASSERT_EQ(data.data.samples.size(), dataAfterSave.data.samples.size());

    for (int i = 0; i < data.data.samples.size(); ++i)
        EXPECT_NEAR(data.data.samples[i], dataAfterSave.data.samples[i], FLOAT_PARSING_ERROR);
}