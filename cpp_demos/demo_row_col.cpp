#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include "matrix.hpp"
#include "matrixexpressionspecialization.hpp"

using namespace bla_ga;
using namespace std::chrono;

// Timing helper
template <typename F>
double timeit(F func, int repeats = 1)
{
    double best_time = 1e9;
    for (int r = 0; r < repeats; ++r)
    {
        auto start = high_resolution_clock::now();
        func();
        auto end = high_resolution_clock::now();
        double t = duration<double>(end - start).count();
        if (t < best_time)
            best_time = t;
    }
    return best_time;
}

// Matrix correctness check using squared tolerance
bool matrices_equal(const Matrix<double> &C, const Matrix<double> &C_exact, double tol = 1e-8)
{
    bool equal = true;
    size_t M = C.nRows();
    size_t N = C.nCols();
    for (size_t j = 0; j < N; ++j)
    {
        if (!equal)
            break;
        for (size_t i = 0; i < M; ++i)
        {
            if (!equal)
                break;

            if ((C(i, j) - C_exact(i, j)) * (C(i, j) - C_exact(i, j)) > tol)
            {
                std::cout << "C(" << i << "," << j << ") = " << C(i, j) << ", C_exact(" << i << "," << j << ") = " << C_exact(i, j) << std::endl;
                equal = false;
            }
        }
    }
    return equal;
}

// Matrix tuning: macro-block + micro-tile
void tune_matrix(size_t M, size_t K, size_t N)
{
    std::vector<double> A_data(M * K, 1.0), B_data(K * N, 1.0), C_data(M * N, 0.0), C_exact_data(M * N, 0.0);
    Matrix<double> A(M, K, A_data.data()), B(K, N, B_data.data()),
        C(M, N, C_data.data()), C_exact(M, N, C_exact_data.data());

    // Exact result
    EvalMatMatMultiplyDouble(A, B, C_exact);

    std::vector<size_t> macro_blocks = {64, 128, 256, 512};

    double best_gflops = 0;
    size_t best_BM = 0, best_BK = 0, best_BN = 0;

    std::cout << "--- Macro-block tuning ---\n";
    for (size_t BM : macro_blocks)
        for (size_t BK : macro_blocks)
            for (size_t BN : macro_blocks)
            {
                // Skip incompatible block sizes
                if (M % BM != 0 || K % BK != 0 || N % BN != 0)
                    continue;

                std::fill(C_data.begin(), C_data.end(), 0.0);
                auto t = timeit([&]
                                { SIMDEvalMatMatMultiplyDouble(A, B, C); });
                double gflops = 2.0 * M * K * N / (t * 1e9);

                bool correct = matrices_equal(C, C_exact);

                std::cout << "[Macro] BM=" << BM << " BK=" << BK << " BN=" << BN
                          << " | Time=" << t << " s | GFLOPS=" << gflops
                          << " | Correct=" << correct << "\n";

                if (correct && gflops > best_gflops)
                {
                    best_gflops = gflops;
                    best_BM = BM;
                    best_BK = BK;
                    best_BN = BN;
                }
            }

    if (best_BM == 0)
    {
        std::cerr << "No valid macro-block passed correctness check! Exiting.\n";
        return;
    }

    std::cout << "Best macro-block: BM=" << best_BM << " BK=" << best_BK << " BN=" << best_BN
              << " | GFLOPS=" << best_gflops << "\n";

    // Micro-tile tuning
    double best_micro_gflops = 0;
    size_t best_W = 4, best_L = 32, best_H = 8;

    std::cout << "--- Micro-tile tuning ---\n";
    for (size_t W : {2, 4, 8})
        for (size_t L : {2, 4, 8, 16, 32})
            for (size_t H : {2, 4, 8})
            {
                // Skip incompatible block sizes

                std::fill(C_data.begin(), C_data.end(), 0.0);
                auto t = timeit([&]
                                { SIMDEvalMatMatMultiplyDouble(A, B, C); });
                double gflops = 2.0 * M * K * N / (t * 1e9);

                bool correct = matrices_equal(C, C_exact);

                std::cout << "[Micro] W=" << W << " L=" << L << " H=" << H
                          << " | Time=" << t << " s | GFLOPS=" << gflops
                          << " | Correct=" << correct << "\n";

                if (correct && gflops > best_micro_gflops)
                {
                    best_micro_gflops = gflops;
                    best_W = W;
                    best_L = L;
                    best_H = H;
                }
            }

    std::cout << "--- Best combination ---\n";
    std::cout << "Macro-block: BM=" << best_BM << " BK=" << best_BK << " BN=" << best_BN << "\n";
    std::cout << "Micro-tile: W=" << best_W << " L=" << best_L << " H=" << best_H << "\n";
    std::cout << "Achieved GFLOPS: " << best_micro_gflops << "\n";
}

int main()
{
    // Test with 1024x1024 matrices
    tune_matrix(1024 / 2, 1024 / 2, 1024 / 2);
    return 0;
}
