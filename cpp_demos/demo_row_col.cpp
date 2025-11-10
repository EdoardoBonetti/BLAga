#include <iostream>
#include <vector>
#include <chrono>
#include "matrix.hpp" // your Matrix class
#include "matrixexpressionspecialization.hpp"

using namespace bla_ga;
using namespace std::chrono;

// Simple timing function
template <typename F>
double timeit(F func, int repeats = 3)
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

// Sweep different block sizes
void tune_block_sizes(size_t M, size_t K, size_t N)
{
    std::vector<double> A_data(M * K, 1.0);
    std::vector<double> B_data(K * N, 1.0);
    std::vector<double> C_data(M * N, 0.0);

    Matrix<double> A(M, K, A_data.data());
    Matrix<double> B(K, N, B_data.data());
    Matrix<double> C(M, N, C_data.data());

    // candidate block sizes (tunable)
    std::vector<size_t> block_sizes = {16, 32, 64, 128, 256, 512};

    double best_gflops = 0;
    size_t best_BM = 0, best_BK = 0, best_BN = 0;

    for (size_t BM : block_sizes)
        for (size_t BK : block_sizes)
            for (size_t BN : block_sizes)
            {
                // Reset C
                std::fill(C_data.begin(), C_data.end(), 0.0);

                auto t = timeit([&]
                                {
                                    SIMDEvalMatMatMultiplyDouble(A, B, C, BM, BK, BN); // you will need to overload function to accept BM,BK,BN
                                });

                double gflops = 2.0 * M * K * N / (t * 1e9);

                std::cout << "BM=" << BM << " BK=" << BK << " BN=" << BN
                          << " | Time=" << t << " s | GFLOPS=" << gflops << "\n";

                if (gflops > best_gflops)
                {
                    best_gflops = gflops;
                    best_BM = BM;
                    best_BK = BK;
                    best_BN = BN;
                }
            }

    std::cout << "Best block sizes: BM=" << best_BM
              << " BK=" << best_BK
              << " BN=" << best_BN
              << " | GFLOPS=" << best_gflops << "\n";
}

int main()
{
    tune_block_sizes(2024, 2024, 2024);
}
