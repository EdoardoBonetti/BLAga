#include <iostream>
#include <chrono>
#include "matrix.hpp"
#include "matrixexpression.hpp"

#include "simd.h"

using namespace bla_ga;

template <size_t BS, size_t RS, typename T = double>
void benchmark_simd(int n)

    template <size_t BS, size_t RS, typename T = double>
    void gemm_simd2(const Matrix<T> &A, const Matrix<T> &B, Matrix<T> &C)
{
    size_t M = A.nRows();
    size_t N = B.nCols();
    size_t K = A.nCols();

    for (size_t ii = 0; ii < M; ii += BS)
        for (size_t kk = 0; kk < K; kk += BS)
            for (size_t jj = 0; jj < N; jj += BS)
                for (size_t i = ii; i < std::min(ii + BS, M); i += RS)
                    for (size_t j = jj; j < std::min(jj + BS, N); j += RS)
                    {
                        size_t tileM = std::min(RS, M - i);
                        size_t tileN = std::min(RS, N - j);

                        // SIMD tile: ceil(tileN/2)
                        SIMD<double, 2> cloc[RS][(RS + 1) / 2];
                        for (size_t ii2 = 0; ii2 < tileM; ++ii2)
                            for (size_t jj2 = 0; jj2 < (tileN + 1) / 2; ++jj2)
                                cloc[ii2][jj2] = SIMD<double, 2>(0.0);

                        // accumulate
                        for (size_t k = kk; k < std::min(kk + BS, K); ++k)
                        {
                            for (size_t ii2 = 0; ii2 < tileM; ++ii2)
                            {
                                SIMD<double, 2> aval(A(i + ii2, k));

                                for (size_t jj2 = 0; jj2 < tileN; jj2 += 2)
                                {
                                    size_t n_valid = std::min<size_t>(2, tileN - jj2);

                                    // Compare indices with scalar for mask
                                    SIMD<int64_t, 2> indices = bla_ga::IndexSequence<int64_t, 2>();
                                    SIMD<mask64, 2> mask = indices < n_valid;

                                    SIMD<double, 2> bvec(&B(k, j + jj2), mask);
                                    cloc[ii2][jj2 / 2] += aval * bvec;
                                }
                            }
                        }

                        // store back
                        for (size_t ii2 = 0; ii2 < tileM; ++ii2)
                            for (size_t jj2 = 0; jj2 < tileN; jj2 += 2)
                            {
                                size_t n_valid = std::min<size_t>(2, tileN - jj2);
                                SIMD<int64_t, 2> indices = bla_ga::IndexSequence<int64_t, 2>();
                                SIMD<mask64, 2> mask = indices < n_valid;

                                cloc[ii2][jj2 / 2].Store(&C(i + ii2, j + jj2), mask);
                            }
                    }
}

int main()
{
    int n = 2501; // matrix size

    benchmark_simd<128, 8>(n);

    return 0;
}
