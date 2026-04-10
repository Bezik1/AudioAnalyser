#include <vector>
#include <cmath>

#include "BenchmarkUtils.hpp"

std::vector<double> BenchmarkUtils::GenerateSignal(int64_t n)
{
    std::vector<double> samples(n);
    for (int64_t i = 0; i < n; ++i)
    {
        samples[i] = std::sin(2.0 * M_PI * i / 44100.0);
    }
    return samples;
}