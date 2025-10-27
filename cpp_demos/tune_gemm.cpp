#include <iostream>
#include <chrono>
#include "matrix.hpp"
#include "matrixexpression.hpp"

using namespace bla_ga;

template <size_t BS, size_t RS, typename T = double>
void benchmark_gemm(int n)
{
    Matrix<T> A(n, n);
    Matrix<T> B(n, n);
    Matrix<T> C(n, n);

    // Initialize matrices
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            A(i, j) = 1.0;
            B(i, j) = 1.0;
            C(i, j) = 0.0;
        }

    auto start = std::chrono::high_resolution_clock::now();

    size_t M = n, N = n, K = n;

    for (size_t ii = 0; ii < M; ii += BS)
        for (size_t kk = 0; kk < K; kk += BS)
            for (size_t jj = 0; jj < N; jj += BS)
                for (size_t i = ii; i < std::min(ii + BS, M); i += RS)
                    for (size_t j = jj; j < std::min(jj + BS, N); j += RS)
                    {
                        size_t tileM = std::min(RS, M - i);
                        size_t tileN = std::min(RS, N - j);

                        // SIMD tile: ceil(tileN / 2)
                        SIMD<double, 2> cloc[RS][(RS + 1) / 2];
                        for (size_t ii2 = 0; ii2 < tileM; ++ii2)
                            for (size_t jj2 = 0; jj2 < (tileN + 1) / 2; ++jj2)
                                cloc[ii2][jj2] = SIMD<double, 2>(0.0);

                        // accumulate
                        for (size_t k = kk; k < std::min(kk + BS, K); ++k)
                        {
                            for (size_t ii2 = 0; ii2 < tileM; ++ii2)
                            {
                                SIMD<double, 2> aval(A(i + ii2, k)); // broadcast A element

                                for (size_t jj2 = 0; jj2 < tileN; jj2 += 2)
                                {
                                    size_t n_valid = std::min<size_t>(2, tileN - jj2);

                                    // create mask for leftover elements
                                    SIMD<int64_t, 2> indices = IndexSequence<int64_t, 2>();
                                    SIMD<int64_t, 2> limit(n_valid);
                                    SIMD<mask64, 2> mask = indices < limit;

                                    SIMD<double, 2> bvec(&B(k, j + jj2), mask); // load with mask
                                    cloc[ii2][jj2 / 2] += aval * bvec;
                                }
                            }
                        }

                        // store back to C
                        for (size_t ii2 = 0; ii2 < tileM; ++ii2)
                            for (size_t jj2 = 0; jj2 < tileN; jj2 += 2)
                            {
                                size_t n_valid = std::min<size_t>(2, tileN - jj2);
                                SIMD<int64_t, 2> indices = IndexSequence<int64_t, 2>();
                                SIMD<int64_t, 2> limit(n_valid);
                                SIMD<mask64, 2> mask = indices < limit;

                                cloc[ii2][jj2 / 2].Store(&C(i + ii2, j + jj2), mask);
                            }
                    }

    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();
    double gflops = 2.0 * n * n * n / (elapsed * 1e9);

    std::cout << "BS=" << BS << " RS=" << RS << " -> Time=" << elapsed
              << " s, GFLOPS=" << gflops << std::endl;
    std::cout << "C(0,0)=" << C(0, 0) << std::endl;
}
int main()
{
    int n = 2501; // matrix size

    benchmark_gemm<16, 4>(n);

    benchmark_gemm<32, 4>(n);

    benchmark_gemm<64, 4>(n);

    benchmark_gemm<128, 4>(n);

    benchmark_gemm<256, 4>(n);

    benchmark_gemm<512, 4>(n);

    benchmark_gemm<1024, 8>(n);
    return 0;
}
