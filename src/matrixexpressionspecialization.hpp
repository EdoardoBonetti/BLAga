
#ifndef FILE_MAT_EXPRESSION_SPECIALIZATION
#define FILE_MAT_EXPRESSION_SPECIALIZATION

#include "matrix_fwd.hpp"
#include "simd.h"
// #include "matrixexpressionspecialization.hpp"
//  #include <type_traits>
#include <algorithm>
#include <cstddef>
#include <mm_malloc.h> // or use aligned_alloc/free
#include <iostream>

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
            for (size_t j = 0; j < b.nCols(); ++j)
            {
                double sum = 0.0;
                for (size_t k = 0; k < a.nCols(); ++k)
                {
                    sum += a(i, k) * b(k, j);
                }
                C(i, j) = sum;
            }
        }
    };

    // Optimized 4x4 micro-kernel using SIMD<double,4>

    void micro_kernel_4x4_packedB_SIMD(
        const double *__restrict__ A,
        const double *__restrict__ packedB,
        double *__restrict__ C,
        size_t lda, size_t ldc,
        size_t M, size_t N, size_t K)
    {
        // SIMD accumulators: c00..c03 for rows 0-1, c10..c13 for rows 2-3
        SIMD<double, 2> c00(0.0), c01(0.0), c02(0.0), c03(0.0);
        SIMD<double, 2> c10(0.0), c11(0.0), c12(0.0), c13(0.0);

        for (size_t k = 0; k < K; ++k)
        {
            // Load rows of A into SIMD registers
            SIMD<double, 2> a0(
                (M > 0) ? A[0 * lda + k] : 0.0,
                (M > 1) ? A[1 * lda + k] : 0.0);

            SIMD<double, 2> a1(
                (M > 2) ? A[2 * lda + k] : 0.0,
                (M > 3) ? A[3 * lda + k] : 0.0);

            // Broadcast each element of B into SIMD registers
            double b0 = (N > 0) ? packedB[0 * K + k] : 0.0;
            double b1 = (N > 1) ? packedB[1 * K + k] : 0.0;
            double b2 = (N > 2) ? packedB[2 * K + k] : 0.0;
            double b3 = (N > 3) ? packedB[3 * K + k] : 0.0;

            // Multiply and accumulate
            if (N > 0)
                c00 = FMA(a0, SIMD<double, 2>(b0, b0), c00);
            if (N > 1)
                c01 = FMA(a0, SIMD<double, 2>(b1, b1), c01);
            if (N > 2)
                c02 = FMA(a0, SIMD<double, 2>(b2, b2), c02);
            if (N > 3)
                c03 = FMA(a0, SIMD<double, 2>(b3, b3), c03);

            if (N > 0)
                c10 = FMA(a1, SIMD<double, 2>(b0, b0), c10);
            if (N > 1)
                c11 = FMA(a1, SIMD<double, 2>(b1, b1), c11);
            if (N > 2)
                c12 = FMA(a1, SIMD<double, 2>(b2, b2), c12);
            if (N > 3)
                c13 = FMA(a1, SIMD<double, 2>(b3, b3), c13);
        }

        // Store SIMD accumulators back to C
        double tmp[2];

        if (N > 0)
        {
            c00.Store(tmp);
            if (M > 0)
                C[0 * ldc + 0] += tmp[0];
            if (M > 1)
                C[1 * ldc + 0] += tmp[1];
        }
        if (N > 1)
        {
            c01.Store(tmp);
            if (M > 0)
                C[0 * ldc + 1] += tmp[0];
            if (M > 1)
                C[1 * ldc + 1] += tmp[1];
        }
        if (N > 2)
        {
            c02.Store(tmp);
            if (M > 0)
                C[0 * ldc + 2] += tmp[0];
            if (M > 1)
                C[1 * ldc + 2] += tmp[1];
        }
        if (N > 3)
        {
            c03.Store(tmp);
            if (M > 0)
                C[0 * ldc + 3] += tmp[0];
            if (M > 1)
                C[1 * ldc + 3] += tmp[1];
        }

        if (N > 0)
        {
            c10.Store(tmp);
            if (M > 2)
                C[2 * ldc + 0] += tmp[0];
            if (M > 3)
                C[3 * ldc + 0] += tmp[1];
        }
        if (N > 1)
        {
            c11.Store(tmp);
            if (M > 2)
                C[2 * ldc + 1] += tmp[0];
            if (M > 3)
                C[3 * ldc + 1] += tmp[1];
        }
        if (N > 2)
        {
            c12.Store(tmp);
            if (M > 2)
                C[2 * ldc + 2] += tmp[0];
            if (M > 3)
                C[3 * ldc + 2] += tmp[1];
        }
        if (N > 3)
        {
            c13.Store(tmp);
            if (M > 2)
                C[2 * ldc + 3] += tmp[0];
            if (M > 3)
                C[3 * ldc + 3] += tmp[1];
        }
    }

    inline static void pack_B_tile(const double *B, size_t ldb, size_t K_block, size_t N_block, double *packedB)
    {
        // Column-major packing for better reuse in kernel
        // B: (K_block × N_block)
        for (size_t j = 0; j < N_block; ++j)
        {
            const double *col = B + j;
            for (size_t k = 0; k < K_block; ++k)
            {
                packedB[j * K_block + k] = col[k * ldb];
            }
        }
        // for (size_t j = 0; j < N_block; ++j)
        //     for (size_t k = 0; k < K_block; ++k)
        //         std::cout << "packedB[" << j * K_block + k << "]=" << packedB[j * K_block + k] << std::endl;
    }

    template <typename MA, typename MB, typename MC>
    inline static void multiply_tiles_packedB_SIMD(
        const MA &A, const MB &B, MC &C,
        size_t i0, size_t j0, size_t k0,
        size_t M_block, size_t N_block, size_t K_block)
    {
        // print block info
        // std::cout << "multiply_tiles_packedB_SIMD: i0=" << i0 << " j0=" << j0 << " k0=" << k0
        //          << " M_block=" << M_block << " N_block=" << N_block << " K_block=" << K_block << std::endl;

        const size_t lda = A.nCols();
        const size_t ldc = C.nCols();

        // 32-byte aligned packed buffer for B
        double *packedB = static_cast<double *>(_mm_malloc(K_block * N_block * sizeof(double), 32));
        pack_B_tile(&B(k0, j0), B.nCols(), K_block, N_block, packedB);

        for (size_t ii = 0; ii < M_block; ii += 4)
        {
            size_t Mtail = std::min((size_t)4, M_block - ii);
            for (size_t jj = 0; jj < N_block; jj += 4)
            {
                size_t Ntail = std::min((size_t)4, N_block - jj);

                double *cptr = &C(i0 + ii, j0 + jj);
                const double *aptr = &A(i0 + ii, k0);
                const double *bptr = packedB + (jj * K_block);

                micro_kernel_4x4_packedB_SIMD(aptr, bptr, cptr, lda, ldc, Mtail, Ntail, K_block);
            }
        }

        _mm_free(packedB);
    }

    template <typename MA, typename MB, typename MC>
    inline static void SIMDEvalMatMatMultiplyDouble(const MA &a, const MB &b, MC &C)
    {
        // std::cout << "Using simple SIMD Eval" << std::endl;
        //  print the matrices:

        // std::cout << "matrix a: \n"
        //           << a << std::endl;
        // std::cout << "matrix b: \n"
        //           << b << std::endl;

        // std::cout << "matrix c: \n"
        //           << C << std::endl;

        size_t M = a.nRows();
        size_t K = a.nCols();
        size_t K2 = b.nRows();
        size_t N = b.nCols();
        if (K != K2)
        {
            std::cerr << "mult_rect: inner dims mismatch: A.cols()=" << K
                      << " B.rows()=" << K2 << std::endl;
            std::exit(1);
        }

#if defined(__arm64__)
        constexpr size_t BM = 64 * 4; // rows of A
        constexpr size_t BK = 64 * 4; // inner dimension
        constexpr size_t BN = 64 * 1; // cols of B
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

                    // std::cout << "Processing block: i=" << ii << " j=" << jj << " k=" << kk
                    //           << " M_block=" << M_block << " N_block=" << N_block << " K_block=" << K_block
                    //           << std::endl;

                    // multiply_tiles_packedB_unrollK_2(A, B, C,
                    //                                ii, jj, kk,
                    //                                M_block, N_block, K_block);
                    multiply_tiles_packedB_SIMD(a, b, C, ii, jj, kk, M_block, N_block, K_block);
                }
    };

} // namespace bla_ga

#endif // FILE_MAT_EXPRESSION_SPECIALIZATION
