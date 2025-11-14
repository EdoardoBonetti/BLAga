
#ifndef FILE_MAT_EXPRESSION_SPECIALIZATION
#define FILE_MAT_EXPRESSION_SPECIALIZATION

// #include "matrix_fwd.hpp"
// #include "matrix.hpp"

#include "simd.h"
// #include "matrixexpressionspecialization.hpp"
//  #include <type_traits>
// #include <algorithm>
// #include <cstddef>
// #include <mm_malloc.h> // or use aligned_alloc/free
#include <iostream>

#include <chrono>
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
            for (size_t k = 0; k < a.nCols(); ++k)
            {
                for (size_t j = 0; j < b.nCols(); ++j)
                {

                    C(i, j) += a(i, k) * b(k, j); // <--- fixed
                }
            }
        }
    };

    // Optimized 4x4 micro-kernel using SIMD<double,4>
    /*
    template <typename MA, typename MB, typename MC>
    inline void EvalMatMatMultiplyDouble_T(const MA &A, const MB &B_transposed, MC &C)
    {
        const size_t M = A.nRows();
        const size_t K = A.nCols();
        const size_t N = B_transposed.nRows(); // rows of packed B == N_block

        for (size_t i = 0; i < M; ++i)
        {

            for (size_t j = 0; j < N; ++j)
            {

                for (size_t k = 0; k < K; ++k)
                {
                    C(i, j) += A(i, k) * B_transposed(j, k); // <--- fixed
                }
            }
        }
    };*/

    /*
    template <size_t Hm, size_t Wm, size_t Lm>
    inline void EvalMatMatMultiplyDouble_Micro(const double *pntA, size_t ldA,
                                               const double *pntBT, size_t ldBT,
                                               double *pntC, size_t ldC)
    {
        // static_assert(Lm % 2 == 0, "L must be a multiple of 2 for float64x2_t SIMD.");

        for (size_t i = 0; i < Hm; ++i)
        {
            const double *rowA = pntA + i * ldA; // row i of A-block
            double *rowC = pntC + i * ldC;       // row i of C-block

            for (size_t j = 0; j < Wm; ++j)
            {
                const double *rowBT = pntBT + j * ldBT; // row j of B^T-block (=> col j of B)

                // --- SIMD accumulate ---
                float64x2_t acc = vdupq_n_f64(0.0);

                // process 2 elements per step
                for (size_t k = 0; k < Lm; k += 2)
                {
                    float64x2_t a = vld1q_f64(&rowA[k]);  // load 2 doubles from A
                    float64x2_t b = vld1q_f64(&rowBT[k]); // load 2 doubles from BT
                    acc = vfmaq_f64(acc, a, b);           // acc += a * b (FMA)
                }

                // horizontal add the 2 lanes of acc
                double sum = vaddvq_f64(acc);

                // accumulate into C
                rowC[j] += sum;
            }
        }
    }
        */

    // template <size_t M = 4, size_t N = 4, size_t L = 8>
    // inline __attribute__((always_inline)) static void EvalMatMatMultiplyDouble_Micro(const double *pntA, size_t ldA,
    //                                                                                  const double *pntBT, size_t ldBT,
    //                                                                                  double *pntC, size_t ldC)
    //
    /*{
        constexpr size_t Step = 4;

        for (size_t i = 0; i < Hm; ++i)
        {
            const double *rowA = pntA + i * ldA;
            double *rowC = pntC + i * ldC;

            for (size_t j = 0; j < Wm; ++j)
            {
                const double *colB = pntBT + j * ldBT;
                SIMD<double, 2> acc1(0.0), acc2(0.0);

                for (size_t k = 0; k < Lm; k += Step)
                {
                    acc1 = FMA(SIMD<double, 2>(&rowA[k + 0]), SIMD<double, 2>(&colB[k + 0]), acc1);
                    acc2 = FMA(SIMD<double, 2>(&rowA[k + 2]), SIMD<double, 2>(&colB[k + 2]), acc2);
                }

                double sum = HSum(acc1 + acc2);

                rowC[j] += sum;
            }
        }
    }*/
    template <size_t M = 4, size_t N = 8, size_t L = 16>
    inline __attribute__((always_inline)) __attribute__((visibility("default"))) void EvalMatMatMultiplyDouble_Micro(
        const double *pntA, size_t ldA,
        const double *pntBT, size_t ldBT,
        double *pntC, size_t ldC)
    {

        constexpr size_t Step = 2; // matches .2d vector width in your assembly

        for (size_t i = 0; i < M; ++i)
        {
            const double *rowA = pntA + i * ldA;
            double *rowC = pntC + i * ldC;

            for (size_t j = 0; j < N; ++j)
            {
                const double *colB = pntBT + j * ldBT;

                // SIMD<double, 2> acc(0.0); // accumulator for 2-wide vector
                //  for (size_t k = 0; k < L; k += Step)
                //{
                //      // mimic ld1r broadcast of A
                //      SIMD<double, 2> a_vec(&rowA[k]); // automatically loads two elements if needed
                //      // load B elements
                //      SIMD<double, 2> b_vec(&colB[k]);
                //      // FMLA: accumulate
                //      acc = FMA(a_vec, b_vec, acc);
                //  }

                SIMD<double, 2> acc(0.0);

                // manual unroll for k = 0..L-1 step Step
                acc = FMA(SIMD<double, 2>(&rowA[0]), SIMD<double, 2>(&colB[0]), acc);
                acc = FMA(SIMD<double, 2>(&rowA[2]), SIMD<double, 2>(&colB[2]), acc);
                acc = FMA(SIMD<double, 2>(&rowA[4]), SIMD<double, 2>(&colB[4]), acc);
                acc = FMA(SIMD<double, 2>(&rowA[6]), SIMD<double, 2>(&colB[6]), acc);
                acc = FMA(SIMD<double, 2>(&rowA[8]), SIMD<double, 2>(&colB[8]), acc);
                acc = FMA(SIMD<double, 2>(&rowA[10]), SIMD<double, 2>(&colB[10]), acc);
                acc = FMA(SIMD<double, 2>(&rowA[12]), SIMD<double, 2>(&colB[12]), acc);
                acc = FMA(SIMD<double, 2>(&rowA[14]), SIMD<double, 2>(&colB[14]), acc);

                // rowC[j] += HSum(acc);

                // horizontal sum to get scalar result
                // double sum = HSum(acc);
                rowC[j] += HSum(acc);
            }
        }
    }

    template <typename MA, typename MB, typename MC> //, size_t W, size_t L, size_t H>
    inline __attribute__((always_inline)) __attribute__((visibility("default"))) void SIMDEvalMatMatMultiplyDouble_Tiles(const MA &A, const MB &B_transposed, MC &C)
    {
        const size_t M = A.nRows();
        // inner dimension (for this sub-block)
        const size_t N = B_transposed.nRows(); // columns of C handled (i.e. rows of packed B)
        const size_t K = A.nCols();

        constexpr size_t W = 4;
        constexpr size_t L = 16;
        constexpr size_t H = 8;

        const size_t ldA = static_cast<size_t>(&A(1, 0) - &A(0, 0));
        const size_t ldBT = static_cast<size_t>(&B_transposed(1, 0) - &B_transposed(0, 0));
        const size_t ldC = static_cast<size_t>(&C(1, 0) - &C(0, 0));

        for (size_t i = 0; i + H <= M; i += H)
        {
            for (size_t j = 0; j + W <= N; j += W)
            {
                for (size_t kk = 0; kk < K; kk += L)
                {
                    //                    EvalMatMatMultiplyDouble_Micro<H, W, L>(
                    //                        &A(i, kk), ldA,
                    //                        &B_transposed(j, kk), ldBT,
                    //                        &C(i, j), ldC);

                    EvalMatMatMultiplyDouble_Micro<H, W, L>(
                        &A(i, kk), ldA,
                        &B_transposed(j, kk), ldBT,
                        &C(i, j), ldC);
                }
            }
        }
    }

    template <typename MA, typename MB, typename MC> //, size_t W = 4, size_t L = 16, size_t H = 4
    inline __attribute__((always_inline)) __attribute__((visibility("default"))) void SIMDEvalMatMatMultiplyDouble(const MA &a, const MB &b, MC &C)
    {

        auto t0 = std::chrono::high_resolution_clock::now();

        size_t M = a.nRows();
        size_t K = a.nCols();
        size_t N = b.nCols();
        /*

                constexpr size_t BM = 512;
                constexpr size_t BK = 64;
                constexpr size_t BN = 256;
        */
        constexpr size_t BM = 256;
        constexpr size_t BK = 16 * 2;
        constexpr size_t BN = 256;

        for (size_t ii = 0; ii < M; ii += BM)
        {
            size_t M_block = std::min(BM, M - ii);
            for (size_t kk = 0; kk < K; kk += BK)
            {
                size_t K_block = std::min(BK, K - kk);
                auto ar = Matrix(a.Rows(ii, ii + M_block).Cols(kk, kk + K_block));

                for (size_t jj = 0; jj < N; jj += BN)
                {
                    size_t N_block = std::min(BN, N - jj);

                    auto cr = C.Rows(ii, ii + M_block).Cols(jj, jj + N_block);

                    auto bTr = PackTransposedBlock(b, kk, jj, K_block, N_block);
                    // auto br = Matrix(b.Rows(kk, kk + K_block).Cols(jj, jj + N_block)).Transposed();
                    SIMDEvalMatMatMultiplyDouble_Tiles(ar, bTr, cr);

                    // auto br = MatrixView(b.Rows(kk, kk + K_block).Cols(jj, jj + N_block));
                    // EvalMatMatMultiplyDouble(ar, br, cr);
                }
            }
        }
        auto t1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = t1 - t0;
        std::cout << "[C++] GEMM time elapsed: " << elapsed.count() << " s\n";
    };

} // namespace bla_ga

#endif // FILE_MAT_EXPRESSION_SPECIALIZATION
