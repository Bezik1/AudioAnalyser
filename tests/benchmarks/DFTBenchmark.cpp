#include <benchmark/benchmark.h>

#include "BenchmarkUtils/BenchmarkUtils.hpp"
#include "../../src/core/AudioAnalyser/AudioAnalyser.hpp"

static void BM_DFT_Complexity(benchmark::State &state)
{
    auto samples = BenchmarkUtils::GenerateSignal(state.range(0));
    for (auto _ : state)
        benchmark::DoNotOptimize(AudioAnalyser::discreteFourierTransform(samples, BenchmarkUtils::SAMPLE_RATE));
    state.SetComplexityN(state.range(0));
}

/**
 * @brief Tests the theoritical correct time complexity of DFT alogirthm, which is O(n^2).
 * 
 */
BENCHMARK(BM_DFT_Complexity)
    ->RangeMultiplier(2)
    ->Range(64, 2048)
    ->Complexity(benchmark::oNSquared);