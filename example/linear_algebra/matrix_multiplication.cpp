#include <armadillo>
using namespace arma;

#include <Eigen/Core>
using namespace Eigen;

#include "benchmark/benchmark.h"

/*------------------------------------------------------------------------------------------------*/
// Data.

constexpr int N = 128;

fmat::fixed<N, N> m0_arma = randu<fmat>(N, N);
fmat::fixed<N, N> m1_arma = randu<fmat>(N, N);
fmat::fixed<N, N> m2_arma = zeros<fmat>(N, N);

Matrix<float, N, N> m0_eigen(m0_arma.mem);
Matrix<float, N, N> m1_eigen(m1_arma.mem);
Matrix<float, N, N> m2_eigen = MatrixXf::Zero(N, N);

float m0_manual[N][N];
float m1_manual[N][N];
float m2_manual[N][N];

/*------------------------------------------------------------------------------------------------*/
// Armadillo.

static void BM_MatMul_Arma_0(benchmark::State &state) {
    for(auto _: state)
        m2_arma = m0_arma * m1_arma;
}
BENCHMARK(BM_MatMul_Arma_0);

static void BM_MatMul_Arma_1(benchmark::State &state) {
    for(auto _: state)
        m2_arma = m0_arma.t() * m1_arma;
}
BENCHMARK(BM_MatMul_Arma_1);

static void BM_MatMul_Arma_2(benchmark::State &state) {
    for(auto _: state)
        m2_arma = m0_arma * m1_arma.t();
}
BENCHMARK(BM_MatMul_Arma_2);

static void BM_MatMul_Arma_3(benchmark::State &state) {
    for(auto _: state)
        m2_arma = m0_arma.t() * m1_arma.t();
}
BENCHMARK(BM_MatMul_Arma_3);

/*------------------------------------------------------------------------------------------------*/
// Eigen.

static void BM_MatMul_Eigen_0(benchmark::State &state) {
    for(auto _: state)
        m2_eigen = m0_eigen * m1_eigen;
}
BENCHMARK(BM_MatMul_Eigen_0);

static void BM_MatMul_Eigen_1(benchmark::State &state) {
    for(auto _: state)
        m2_eigen = m0_eigen.transpose() * m1_eigen;
}
BENCHMARK(BM_MatMul_Eigen_1);

static void BM_MatMul_Eigen_2(benchmark::State &state) {
    for(auto _: state)
        m2_eigen = m0_eigen * m1_eigen.transpose();
}
BENCHMARK(BM_MatMul_Eigen_2);

static void BM_MatMul_Eigen_3(benchmark::State &state) {
    for(auto _: state)
        m2_eigen = m0_eigen.transpose() * m1_eigen.transpose();
}
BENCHMARK(BM_MatMul_Eigen_3);

/*------------------------------------------------------------------------------------------------*/
// Manual.

static void BM_MatMul_Manual_0(benchmark::State &state) {
    for(auto _: state) {
        for(int i_row = 0; i_row < N; ++i_row)
            for(int i_col = 0; i_col < N; ++i_col)
                for(int k = 0; k < N; ++k)
                    m2_manual[i_row][i_col] += m0_manual[i_row][k] * m1_manual[k][i_col];
    }
}
BENCHMARK(BM_MatMul_Manual_0);

static void BM_MatMul_Manual_1(benchmark::State &state) {
    for(auto _: state) {
        for(int i_row = 0; i_row < N; ++i_row)
            for(int k = 0; k < N; ++k)
                for(int i_col = 0; i_col < N; ++i_col)
                    m2_manual[i_row][i_col] += m0_manual[i_row][k] * m1_manual[k][i_col];
    }
}
BENCHMARK(BM_MatMul_Manual_1);

static void BM_MatMul_Manual_2(benchmark::State &state) {
    for(auto _: state) {
        for(int i_row = 0; i_row < N; ++i_row)
            for(int k = 0; k < N; ++k) {
                float e_m0_manual = m0_manual[i_row][k];
                for(int i_col = 0; i_col < N; ++i_col)
                    m2_manual[i_row][i_col] += e_m0_manual * m1_manual[k][i_col];
            }
    }
}
BENCHMARK(BM_MatMul_Manual_2);

/*------------------------------------------------------------------------------------------------*/
// Init.

// BENCHMARK_MAIN();

int main(int argc, char *argv[]) {
    memcpy(m0_manual, m0_arma.memptr(), N * N);
    memcpy(m1_manual, m1_arma.memptr(), N * N);
    memset(m2_manual, 0, N * N);

    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();
}

/*------------------------------------------------------------------------------------------------*/
/* Without -march=native.

Run on (8 X 4200 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 8192 KiB (x1)
Load Average: 3.17, 4.18, 3.95
-------------------------------------------------------------
Benchmark                   Time             CPU   Iterations
-------------------------------------------------------------
BM_MatMul_Arma_0        30479 ns        29104 ns        25940
BM_MatMul_Arma_1        28009 ns        26815 ns        25558
BM_MatMul_Arma_2        30502 ns        27851 ns        24467
BM_MatMul_Arma_3        29801 ns        28225 ns        24515
BM_MatMul_Eigen_0      142370 ns       141698 ns         4445
BM_MatMul_Eigen_1      142383 ns       141653 ns         4964
BM_MatMul_Eigen_2      144278 ns       143615 ns         4873
BM_MatMul_Eigen_3      144265 ns       143760 ns         4879
BM_MatMul_Manual_0    1612288 ns      1606306 ns          431
BM_MatMul_Manual_1     158825 ns       158303 ns         4405
BM_MatMul_Manual_2     159906 ns       159182 ns         4411

*/

/*------------------------------------------------------------------------------------------------*/
/* With -march=native.

Run on (8 X 4200 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 8192 KiB (x1)
Load Average: 2.64, 3.79, 3.88
-------------------------------------------------------------
Benchmark                   Time             CPU   Iterations
-------------------------------------------------------------
BM_MatMul_Arma_0        27628 ns        27323 ns        25432
BM_MatMul_Arma_1        28800 ns        28415 ns        24107
BM_MatMul_Arma_2        30992 ns        30443 ns        23324
BM_MatMul_Arma_3        29571 ns        29180 ns        23135
BM_MatMul_Eigen_0       50309 ns        50310 ns        13879
BM_MatMul_Eigen_1       50713 ns        50714 ns        13759
BM_MatMul_Eigen_2       54583 ns        54583 ns        12704
BM_MatMul_Eigen_3       58892 ns        58891 ns        11828
BM_MatMul_Manual_0    1597445 ns      1597476 ns          436
BM_MatMul_Manual_1      73072 ns        73073 ns         9497
BM_MatMul_Manual_2      73581 ns        73582 ns         9537

*/
