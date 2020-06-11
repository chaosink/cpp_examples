#include <random>

#include <immintrin.h>

#include "benchmark/benchmark.h"

constexpr int n_batch = 1024;
constexpr int n = n_batch * 8;

// #pragma pack(32) // This only has effect on struct/union/class members, but no effect on arrays.
alignas(32) float a[n]; // `alignas` is a C++11 specifier.
alignas(32) float b[n];
alignas(32) float c[n];

static void BM_Scalar(benchmark::State &state) {
    for(auto _: state)
        for(int i = 0; i < n; ++i)
            c[i] = a[i] * b[i];
}
BENCHMARK(BM_Scalar);

static void BM_Packet_4(benchmark::State &state) {
    for(auto _: state) {
        for(int i = 0; i < n; i += 4) {
            __m128 av = _mm_load_ps(a + i);
            __m128 bv = _mm_load_ps(b + i);
            __m128 cv = _mm_mul_ps(av, bv);
            _mm_store_ps(c + i, cv);
        }
    }
}
BENCHMARK(BM_Packet_4);

static void BM_Packet_8(benchmark::State &state) {
    for(auto _: state) {
        for(int i = 0; i < n; i += 8) {
            __m256 av = _mm256_load_ps(a + i);
            __m256 bv = _mm256_load_ps(b + i);
            __m256 cv = _mm256_mul_ps(av, bv);
            _mm256_store_ps(c + i, cv);
        }
    }
}
BENCHMARK(BM_Packet_8);

// BENCHMARK_MAIN();

int main(int argc, char *argv[]) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.f, 1.f);
    for(int i = 0; i < n; ++i) {
        a[i] = dist(gen);
        b[i] = dist(gen);
    }

    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();
}

/*--------------------------------------------------------------------------------------------------
// Debug mode without `-march=native`.

Run on (8 X 4200 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 8192 KiB (x1)
Load Average: 1.62, 1.54, 1.16
***WARNING*** Library was built as DEBUG. Timings may be affected.
------------------------------------------------------
Benchmark            Time             CPU   Iterations
------------------------------------------------------
BM_Scalar        14495 ns        14495 ns        47988
BM_Packet_4       9284 ns         9283 ns        75391
BM_Packet_8       4585 ns         4585 ns       153574

*/

/*--------------------------------------------------------------------------------------------------
// Release mode with `-march=native`.

Run on (8 X 4200 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 8192 KiB (x1)
Load Average: 1.56, 1.47, 1.09
------------------------------------------------------
Benchmark            Time             CPU   Iterations
------------------------------------------------------
BM_Scalar          805 ns          805 ns       849604
BM_Packet_4       1222 ns         1222 ns       575117
BM_Packet_8        796 ns          796 ns       849685

*/
