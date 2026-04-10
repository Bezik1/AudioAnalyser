#include <benchmark/benchmark.h>

#include "BenchmarkUtils/BenchmarkUtils.hpp"
#include "../../src/core/AudioAnalyser/AudioAnalyser.hpp"

static void BM_FFT_Complexity(benchmark::State &state)
{
    auto samples = BenchmarkUtils::GenerateSignal(state.range(0));
    for (auto _ : state)
        benchmark::DoNotOptimize(AudioAnalyser::fastFourierTransform(samples, BenchmarkUtils::SAMPLE_RATE));
    state.SetComplexityN(state.range(0));
}

/**
 * @brief Tests the theoritical correct time complexity of FFT alogirthm, which is O(nlog{n}).
 *
 */
BENCHMARK(BM_FFT_Complexity)
    ->RangeMultiplier(2)
    ->Range(64, 1 << 15)
    ->Complexity(benchmark::oNLogN);