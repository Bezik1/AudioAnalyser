#include <vector>
#include <cmath>

namespace BenchmarkUtils
{
    constexpr int SAMPLE_RATE = 44100;
    std::vector<double> GenerateSignal(int64_t n);
}