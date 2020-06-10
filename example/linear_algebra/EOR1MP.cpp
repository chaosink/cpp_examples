// EOR1MP (Economic Orthogonal Rank-One Matrix Pursuit)
// 2015 - SJSC - Orthogonal Rank-One Matrix Pursuit for Low Rank Matrix Completion
// https://github.com/andrewssobral/mctc4bmi/blob/master/algs_mc/OR1MP/EOR1MP.m

/*------------------------------------------------------------------------------------------------*/
// .hpp

#include <armadillo>

namespace eor1mp {

using Float = float;

struct Data {
    size_t n_row, n_col, n_elem;
    unsigned n_basis;
    std::vector<size_t> index;
    arma::Mat<Float> Y_i, Y_o, U, V;
    arma::Col<Float> theta;

    Data(size_t n_row,
         size_t n_col,
         unsigned n_basis,
         std::vector<size_t> _index,
         const std::vector<Float> &data);
};

class EOR1MP {
    static void TopSVD(arma::subview_col<Float> &u,
                       Float &s,
                       arma::subview_col<Float> &v,
                       const arma::Mat<Float> &m,
                       unsigned round);

    static void SparseMul(arma::subview_col<Float> m,
                          const arma::subview_col<Float> &u,
                          const arma::subview_col<Float> &v,
                          const std::vector<size_t> &index);

    static void SparseMul(arma::subview_col<Float> m,
                          const arma::Mat<Float> &u,
                          const arma::Col<Float> &v,
                          const std::vector<size_t> &index);

public:
    static void Solve(Data &data);
};

} // namespace eor1mp

namespace eor1mp_sparse {

using Float = float;

struct Data {
    size_t n_row, n_col, n_elem;
    unsigned n_basis;
    std::vector<size_t> index;
    std::vector<Float> data;
    arma::Mat<Float> Y_i, Y_o, U, V;
    arma::Col<Float> theta;

    Data(size_t n_row,
         size_t n_col,
         unsigned n_basis,
         std::vector<size_t> _index,
         std::vector<Float> _data);
};

class EOR1MP {
    static void TopSVD(arma::subview_col<Float> &u,
                       Float &s,
                       arma::subview_col<Float> &v,
                       const arma::SpMat<Float> &m,
                       unsigned round);

    static void SparseMul(arma::subview_col<Float> m,
                          const arma::subview_col<Float> &u,
                          const arma::subview_col<Float> &v,
                          const std::vector<size_t> &index);

    static void SparseAssign(arma::SpMat<Float> &m,
                             arma::Col<Float> &data,
                             std::vector<size_t> &index);

public:
    static void Solve(Data &data);
};

} // namespace eor1mp_sparse

/*------------------------------------------------------------------------------------------------*/
// .cpp

namespace eor1mp {

Data::Data(size_t n_row,
           size_t n_col,
           unsigned n_basis,
           std::vector<size_t> _index,
           const std::vector<Float> &data)
    : n_row(n_row)
    , n_col(n_col)
    , n_elem(n_row * n_col)
    , n_basis(n_basis)
    , index(std::move(_index))
    , Y_i(n_row, n_col, arma::fill::zeros)
    , U(n_row, n_basis)
    , V(n_col, n_basis)
    , theta(n_basis) {
    for(size_t i = 0; i < index.size(); ++i)
        Y_i[index[i]] = data[i];
}

void EOR1MP::TopSVD(arma::subview_col<Float> &u,
                    Float &s,
                    arma::subview_col<Float> &v,
                    const arma::Mat<Float> &m,
                    unsigned round) {
    Float stop_eps = 1e-3f;
    u.ones();
    arma::Col<Float> v_last(v.n_rows, arma::fill::zeros);
    Float u_norm, v_norm;

    for(unsigned i = 0; i < round; ++i) {
        u_norm = arma::norm(u);
        v = m.t() * u / (u_norm * u_norm);
        v_norm = arma::norm(v);
        u = m * v / (v_norm * v_norm);
        if(arma::norm(v - v_last) < stop_eps)
            break;
        v_last = v;
    }

    u = u / u_norm;
    v = v / v_norm;
    s = u_norm * v_norm;
}

void EOR1MP::SparseMul(arma::subview_col<Float> m,
                       const arma::subview_col<Float> &u,
                       const arma::subview_col<Float> &v,
                       const std::vector<size_t> &index) {
    for(auto i: index)
        m[i] = u[i % u.n_rows] * v[i / u.n_rows];
}

void EOR1MP::SparseMul(arma::subview_col<Float> m,
                       const arma::Mat<Float> &u,
                       const arma::Col<Float> &v,
                       const std::vector<size_t> &index) {
    for(auto i: index)
        m[i] = arma::dot(u.row(i), v);
}

void EOR1MP::Solve(Data &data) {
    arma::Col<Float> Y(data.Y_i.mem, data.n_elem);
    for(auto i: data.index)
        if(!Y[i])
            // Convert zero to epsilon, according the MATLAB code.
            Y[i] = std::numeric_limits<Float>::epsilon();

    arma::Mat<Float> XM(data.n_elem, 2, arma::fill::zeros);
    size_t i_X = 1, i_M = 0;
    arma::Mat<Float> R(data.n_elem, 1);
    Float s;
    arma::Col<Float>::fixed<2> alpha;

    for(unsigned i_basis = 0; i_basis < data.n_basis; ++i_basis) {
        // 1. Find the top singular pair of the residual.
        R = Y - XM.col(i_X);
        R.reshape(data.n_row, data.n_col);
        arma::subview_col<Float> u = data.U.col(i_basis);
        arma::subview_col<Float> v = data.V.col(i_basis);
        TopSVD(u, s, v, R, 20); // Iteration count `20` from the MATLAB code.

        // 2. Update the weight `theta`, the pursuit basis is `uv'`, its weight is `s`.
        SparseMul(XM.col(i_M), u, v, data.index);
        alpha = arma::solve(XM, Y);
        data.theta[i_basis] = alpha[i_M];
        if(i_basis)
            data.theta.rows(0, i_basis - 1) *= alpha[i_X];
        SparseMul(XM.col(i_M), XM, alpha, data.index);

        std::swap(i_X, i_M);
    }

    data.Y_o = data.U * arma::diagmat(data.theta) * data.V.t();
}

} // namespace eor1mp

namespace eor1mp_sparse {

Data::Data(size_t n_row,
           size_t n_col,
           unsigned n_basis,
           std::vector<size_t> _index,
           std::vector<Float> _data)
    : n_row(n_row)
    , n_col(n_col)
    , n_elem(n_row * n_col)
    , n_basis(n_basis)
    , index(std::move(_index))
    , data(std::move(_data))
    , Y_i(n_row, n_col, arma::fill::zeros)
    , U(n_row, n_basis)
    , V(n_col, n_basis)
    , theta(n_basis) {
    for(size_t i = 0; i < index.size(); ++i)
        Y_i[index[i]] = data[i];
}

void EOR1MP::TopSVD(arma::subview_col<Float> &u,
                    Float &s,
                    arma::subview_col<Float> &v,
                    const arma::SpMat<Float> &m,
                    unsigned round) {
    Float stop_eps = 1e-3f;
    u.ones();
    arma::Col<Float> v_last(v.n_rows, arma::fill::zeros);
    Float u_norm, v_norm;

    for(unsigned i = 0; i < round; ++i) {
        u_norm = arma::norm(u);
        v = m.t() * u / (u_norm * u_norm);
        v_norm = arma::norm(v);
        u = m * v / (v_norm * v_norm);
        if(arma::norm(v - v_last) < stop_eps)
            break;
        v_last = v;
    }

    u = u / u_norm;
    v = v / v_norm;
    s = u_norm * v_norm;
}

void EOR1MP::SparseMul(arma::subview_col<Float> m,
                       const arma::subview_col<Float> &u,
                       const arma::subview_col<Float> &v,
                       const std::vector<size_t> &index) {
    for(size_t i = 0; i < index.size(); ++i)
        m[i] = u[index[i] % u.n_rows] * v[index[i] / u.n_rows];
}

void EOR1MP::SparseAssign(arma::SpMat<Float> &m,
                          arma::Col<Float> &data,
                          std::vector<size_t> &index) {
    for(size_t i = 0; i < index.size(); ++i)
        m[index[i]] = data[i];
}

void EOR1MP::Solve(Data &data) {
    arma::Col<Float> Y(data.data.data(), data.data.size());
    for(auto &y: Y)
        if(!y)
            // Convert zero to epsilon, according the MATLAB code.
            y = std::numeric_limits<Float>::epsilon();

    arma::Mat<Float> XM(data.data.size(), 2, arma::fill::zeros);
    size_t i_X = 1, i_M = 0;
    arma::Col<Float> R(data.data.size());
    arma::Col<Float> S(1);
    arma::Col<Float>::fixed<2> alpha;

    arma::umat location(2, data.index.size());
    for(size_t i = 0; i < data.index.size(); ++i) {
        location[i * 2 + 0] = data.index[i] % data.n_row;
        location[i * 2 + 1] = data.index[i] / data.n_row;
    }
    arma::SpMat<Float> R_sparse(location, R, data.n_row, data.n_col);

    for(unsigned i_basis = 0; i_basis < data.n_basis; ++i_basis) {
        // 1. Find the top singular pair of the residual.
        R = Y - XM.col(i_X);
        SparseAssign(R_sparse, R, data.index);
        arma::subview_col<Float> u = data.U.col(i_basis);
        arma::subview_col<Float> v = data.V.col(i_basis);

        constexpr bool USE_ARMA_SVD = false;
        if constexpr(USE_ARMA_SVD) {
            arma::Mat<Float> U(u.colptr(0), u.n_rows, 1, false);
            arma::Mat<Float> V(v.colptr(0), v.n_rows, 1, false);
            arma::svds(U, S, V, R_sparse, 1); // Truncated SVD from Armadillo.
        } else {
            TopSVD(u, S[0], v, R_sparse, 20); // Iteration count `20` from the MATLAB code.
        }

        // 2. Update the weight `theta`, the pursuit basis is `uv'`, its weight is `s`.
        SparseMul(XM.col(i_M), u, v, data.index);
        alpha = arma::solve(XM, Y);
        data.theta[i_basis] = alpha[i_M];
        if(i_basis)
            data.theta.rows(0, i_basis - 1) *= alpha[i_X];
        XM.col(i_M) = XM * alpha;

        std::swap(i_X, i_M);
    }

    data.Y_o = data.U * arma::diagmat(data.theta) * data.V.t();
}

} // namespace eor1mp_sparse

/*------------------------------------------------------------------------------------------------*/
// test

#include <iostream>
using namespace std;

#include "benchmark/benchmark.h"

#define Print(x) std::cout << #x << " =" << std::endl << x << std::endl;

static void BM_EOR1MP_2x2(benchmark::State &state) {
    using namespace eor1mp;

    Data data(2, 2, 10, {1, 2, 3}, {1, 2, 3});

    for(auto _: state)
        EOR1MP::Solve(data);
}
BENCHMARK(BM_EOR1MP_2x2);

static void BM_EOR1MP_5x5(benchmark::State &state) {
    using namespace eor1mp;

    // clang-format off
    Data data(5, 5, 10,
              {
                        0,       1,       2,                4,
                        5,                                  9,
                                         12,      13,
                                16,      17,
                       20,      21,
              },
              {
                  82.0000, 14.0000, 12.0000,          33.0000,
                  95.0000,                            27.0000,
                                    92.0000, 83.0000,
                           47.0000, 59.0000,
                  23.0000, 26.0000,
              });
    // clang-format on

    for(auto _: state)
        EOR1MP::Solve(data);
}
BENCHMARK(BM_EOR1MP_5x5);

static void BM_EOR1MP_SPARSE_2x2(benchmark::State &state) {
    using namespace eor1mp_sparse;

    Data data(2, 2, 10, {1, 2, 3}, {1, 2, 3});

    for(auto _: state)
        EOR1MP::Solve(data);
}
BENCHMARK(BM_EOR1MP_SPARSE_2x2);

static void BM_EOR1MP_SPARSE_5x5(benchmark::State &state) {
    using namespace eor1mp_sparse;

    // clang-format off
    Data data(5, 5, 10,
              {
                        0,       1,       2,                4,
                        5,                                  9,
                                         12,      13,
                                16,      17,
                       20,      21,
              },
              {
                  82.0000, 14.0000, 12.0000,          33.0000,
                  95.0000,                            27.0000,
                                    92.0000, 83.0000,
                           47.0000, 59.0000,
                  23.0000, 26.0000,
              });
    // clang-format on

    for(auto _: state)
        EOR1MP::Solve(data);
}
BENCHMARK(BM_EOR1MP_SPARSE_5x5);

// BENCHMARK_MAIN();

int main(int argc, char *argv[]) {
    using namespace std;

    {
        using namespace eor1mp;
        cout << "eor1mp" << endl << endl;

        {
            Data data(2, 2, 10, {1, 2, 3}, {1, 2, 3});
            Print(data.Y_i.t());

            EOR1MP::Solve(data);
            Print(data.Y_o.t());

            EOR1MP::Solve(data); // Multiple runs.
            Print(data.Y_o.t());
        }

        cout << "------------------------------" << endl;

        {
            // clang-format off
            Data data(5, 5, 10,
                      {
                                0,       1,       2,                4,
                                5,                                  9,
                                                 12,      13,
                                        16,      17,
                               20,      21,
                      },
                      {
                          82.0000, 14.0000, 12.0000,          33.0000,
                          95.0000,                            27.0000,
                                            92.0000, 83.0000,
                                   47.0000, 59.0000,
                          23.0000, 26.0000,
                      });
            // clang-format on
            Print(data.Y_i.t());

            EOR1MP::Solve(data);
            Print(data.Y_o.t());
        }
    }

    cout << "------------------------------------------------------------" << endl;

    {
        using namespace eor1mp_sparse;
        cout << "eor1mp_sparse" << endl << endl;

        {
            Data data(2, 2, 10, {1, 2, 3}, {1, 2, 3});
            Print(data.Y_i.t());

            EOR1MP::Solve(data);
            Print(data.Y_o.t());

            EOR1MP::Solve(data); // Multiple runs.
            Print(data.Y_o.t());
        }

        cout << "------------------------------" << endl;

        {
            // clang-format off
            Data data(5, 5, 10,
                      {
                                0,       1,       2,                4,
                                5,                                  9,
                                                 12,      13,
                                        16,      17,
                               20,      21,
                      },
                      {
                          82.0000, 14.0000, 12.0000,          33.0000,
                          95.0000,                            27.0000,
                                            92.0000, 83.0000,
                                   47.0000, 59.0000,
                          23.0000, 26.0000,
                      });
            // clang-format on
            Print(data.Y_i.t());

            EOR1MP::Solve(data);
            Print(data.Y_o.t());
        }
    }

    cout << "------------------------------------------------------------" << endl;

    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();

    return 0;
}
