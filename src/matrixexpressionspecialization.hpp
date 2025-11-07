
#ifndef FILE_MAT_EXPRESSION_SPECIALIZATION
#define FILE_MAT_EXPRESSION_SPECIALIZATION

#include "matrix_fwd.hpp"
#include "simd.h"
// #include "matrixexpressionspecialization.hpp"
//  #include <type_traits>
// #include <algorithm>
// #include <cstddef>
// #include <mm_malloc.h> // or use aligned_alloc/free
// #include <iostream>

// for std::min

#ifdef _OPENMP
#include <omp.h>
#endif

namespace bla_ga
{

    template <typename MA, typename MB, typename MC>
    inline void EvalMatMatMultiplyDouble(const MA &a, const MB &b, MC &C)
    {

        for (size_t i = 0; i < a.nRows(); ++i)
        {
            for (size_t k = 0; k < a.nCols(); ++k)
            {
                double sum = 0.0;

                for (size_t j = 0; j < b.nCols(); ++j)
                {
                    C(i, j) += a(i, k) * b(k, j);
                }
            }
        }
    };

    // Optimized 4x4 micro-kernel using SIMD<double,4>

    template <typename MA, typename MB, typename MC>
    inline static void SIMDEvalMatMatMultiplyDouble(const MA &a, const MB &b, MC &C)
    {

        size_t M = a.nRows();
        size_t K = a.nCols();
        size_t N = b.nCols();

#if defined(__arm64__)
        constexpr size_t BM = 32 * 2; // rows of A
        constexpr size_t BK = 16;     // inner dimension
        constexpr size_t BN = 32 * 2; // cols of B
#elif defined(__AVX2__)
        // intel
        constexpr size_t BM = 128;
        constexpr size_t BN = 128;
        constexpr size_t BK = 64;
#else
        //
        constexpr size_t BM = 128;
        constexpr size_t BN = 128;
        constexpr size_t BK = 64;
#endif

        for (size_t ii = 0; ii < M; ii += BM)
            for (size_t kk = 0; kk < K; kk += BK)
                for (size_t jj = 0; jj < N; jj += BN)

                {

                    size_t M_block = std::min(BM, M - ii);
                    size_t N_block = std::min(BN, N - jj);
                    size_t K_block = std::min(BK, K - kk);

                    auto ar = MatrixView(a.Rows(ii, ii + M_block).Cols(kk, kk + K_block));
                    auto br = Matrix(b.Rows(kk, kk + K_block).Cols(jj, jj + N_block).Transpose());
                    // auto br = Matrix(b.Rows(kk, kk + K_block).Cols(jj, jj + N_block));

                    auto subC = C.Rows(ii, ii + M_block).Cols(jj, jj + N_block);
                    EvalMatMatMultiplyDouble(ar, br, subC);

                    // multiply_tiles_packedB_SIMD(a, b, C, ii, jj, kk, M_block, N_block, K_block);
                }
    };

} // namespace bla_ga

#endif // FILE_MAT_EXPRESSION_SPECIALIZATION
