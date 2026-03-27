#include <gtest/gtest.h>
#include "../../src/utils/AudioUtils/AudioUtils.hpp"

class AudioUtilsTest : public testing::Test
{
protected:
    AudioUtils audioUtils;
};

TEST_F(AudioUtilsTest, WavReadingTest)
{
    auto data = audioUtils.readWav("test/test.wav");

    ASSERT_EQ(data.chunkID.size(), 4);
    ASSERT_EQ(data.chunkID, "RIFF");

    ASSERT_EQ(data.format.size(), 4);
    ASSERT_EQ(data.format, "WAVE");
}