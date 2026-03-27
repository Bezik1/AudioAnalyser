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

    EXPECT_GT(data.getNumSamples(), 0);
    EXPECT_GT(data.sampleRate, 0);
}