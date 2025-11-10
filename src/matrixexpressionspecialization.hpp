
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
                for (size_t j = 0; j < b.nCols(); ++j)
                {

                    C(i, j) += a(i, k) * b(k, j); // <--- fixed
                }
            }
        }
    };

    // Optimized 4x4 micro-kernel using SIMD<double,4>

    template <typename MA, typename MB, typename MC>
    inline static void SIMDEvalMatMatMultiplyDouble(const MA &a, const MB &b, MC &C, size_t BM = 512, size_t BK = 64, size_t BN = 128)
    {

        size_t M = a.nRows();
        size_t K = a.nCols();
        size_t N = b.nCols();

        // #if defined(__arm64__)
        //        constexpr size_t BM = 32 * 1; // rows of A
        //        constexpr size_t BK = 32 * 6; // inner dimension
        //        constexpr size_t BN = 32 * 6; // cols of B
        // #elif defined(__AVX2__)
        //        // intel
        //        constexpr size_t BM = 128;
        //        constexpr size_t BN = 128;
        //        constexpr size_t BK = 64;
        // #else
        //        //
        //        constexpr size_t BM = 128;
        //        constexpr size_t BN = 128;
        //        constexpr size_t BK = 64;
        // #endif

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

                    auto br = PackTransposedBlock(b, kk, jj, K_block, N_block);
                    // auto br = Matrix(b.Rows(kk, kk + K_block).Cols(jj, jj + N_block));
                    EvalMatMatMultiplyDouble_T2_unrolled(ar, br, cr);

                    // auto br = MatrixView(b.Rows(kk, kk + K_block).Cols(jj, jj + N_block));
                    // EvalMatMatMultiplyDouble(ar, br, cr);
                }
            }
        }
    };

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
    }
    template <typename MA, typename MB, typename MC>
    inline void EvalMatMatMultiplyDouble_T2(const MA &A, const MB &B_transposed, MC &C)
    {
        using namespace bla_ga;
        const size_t M = A.nRows();
        const size_t K = A.nCols();
        const size_t N = B_transposed.nRows(); // packed: rows == original N_block

        double *Cdata = C.Data();
        const double *Adata = A.Data();
        const double *Bdata = B_transposed.Data();

        const size_t lda = A.Dist();
        const size_t ldb = B_transposed.Dist();
        const size_t ldc = C.Dist();

        // Main 2x2 blocked loop
        size_t i = 0;
        for (; i + 1 < M; i += 2)
        {
            double *C_row0 = Cdata + i * ldc;
            double *C_row1 = Cdata + (i + 1) * ldc;
            const double *A_row0 = Adata + i * lda;
            const double *A_row1 = Adata + (i + 1) * lda;

            size_t j = 0;
            for (; j + 1 < N; j += 2)
            {
                const double *B_row0 = Bdata + j * ldb;       // B_transposed row j (K elems)
                const double *B_row1 = Bdata + (j + 1) * ldb; // B_transposed row j+1

                // accumulators as SIMD<double,2>
                SIMD<double, 2> c00(0.0), c01(0.0), c10(0.0), c11(0.0);

                // k loop in steps of 2 (each SIMD holds two k-values)
                size_t k = 0;
                for (; k + 1 < K; k += 2)
                {
                    // Load two consecutive elements from A rows and B rows into SIMD vectors
                    SIMD<double, 2> a0(A_row0[k], A_row0[k + 1]); // [A(i,k), A(i,k+1)]
                    SIMD<double, 2> a1(A_row1[k], A_row1[k + 1]); // [A(i+1,k), A(i+1,k+1)]

                    SIMD<double, 2> b0(B_row0[k], B_row0[k + 1]); // [B_T(j,k), B_T(j,k+1)]
                    SIMD<double, 2> b1(B_row1[k], B_row1[k + 1]); // [B_T(j+1,k), B_T(j+1,k+1)]

                    // FMA: c += a * b  (vector lane-wise)
                    c00 = FMA(a0, b0, c00);
                    c01 = FMA(a0, b1, c01);
                    c10 = FMA(a1, b0, c10);
                    c11 = FMA(a1, b1, c11);
                }

                // Handle leftover k if K is odd (scalar fallback)
                if (k < K)
                {
                    double a0s = A_row0[k];
                    double a1s = A_row1[k];
                    double b0s = B_row0[k];
                    double b1s = B_row1[k];

                    c00 = c00 + SIMD<double, 2>(a0s * b0s);
                    c01 = c01 + SIMD<double, 2>(a0s * b1s);
                    c10 = c10 + SIMD<double, 2>(a1s * b0s);
                    c11 = c11 + SIMD<double, 2>(a1s * b1s);
                }

                // Horizontal sum of SIMD accumulators -> scalar contributions
                double s00 = HSum(c00);
                double s01 = HSum(c01);
                double s10 = HSum(c10);
                double s11 = HSum(c11);

                // Store back to C (accumulate)
                C_row0[j] += s00;
                C_row0[j + 1] += s01;
                C_row1[j] += s10;
                C_row1[j + 1] += s11;
            } // end j loop (j+1 < N)

            // Handle tail j if N is odd (compute C(i, N-1) and C(i+1, N-1))
            for (; j < N; ++j)
            {
                const double *B_row = Bdata + j * ldb;
                double sum0 = 0.0;
                double sum1 = 0.0;
                for (size_t k = 0; k < K; ++k)
                {
                    sum0 += A_row0[k] * B_row[k];
                    sum1 += A_row1[k] * B_row[k];
                }
                C_row0[j] += sum0;
                C_row1[j] += sum1;
            }
        } // end i loop

        // Handle leftover last row if M is odd
        for (; i < M; ++i)
        {
            double *C_row = Cdata + i * ldc;
            const double *A_row = Adata + i * lda;
            for (size_t j = 0; j < N; ++j)
            {
                const double *B_row = Bdata + j * ldb;
                double sum = 0.0;
                for (size_t k = 0; k < K; ++k)
                    sum += A_row[k] * B_row[k];
                C_row[j] += sum;
            }
        }
    };

    template <typename MA, typename MB, typename MC>
    inline void EvalMatMatMultiplyDouble_T2_unrolled(const MA &A, const MB &B_transposed, MC &C)
    {
        using namespace bla_ga;
        const size_t M = A.nRows();
        const size_t K = A.nCols();
        const size_t N = B_transposed.nRows(); // packed: rows == original N_block

        double *Cdata = C.Data();
        const double *Adata = A.Data();
        const double *Bdata = B_transposed.Data();

        const size_t lda = A.Dist();
        const size_t ldb = B_transposed.Dist();
        const size_t ldc = C.Dist();

        // Process 2x2 C tiles (i,i+1) x (j,j+1)
        size_t i = 0;
        for (; i + 1 < M; i += 2)
        {
            double *C_row0 = Cdata + i * ldc;
            double *C_row1 = Cdata + (i + 1) * ldc;
            const double *A_row0 = Adata + i * lda;
            const double *A_row1 = Adata + (i + 1) * lda;

            size_t j = 0;
            for (; j + 1 < N; j += 2)
            {
                const double *B_row0 = Bdata + j * ldb;       // B_transposed row j (K elems)
                const double *B_row1 = Bdata + (j + 1) * ldb; // B_transposed row j+1

                // Two accumulators per output to increase ILP
                SIMD<double, 2> c00a(0.0), c00b(0.0);
                SIMD<double, 2> c01a(0.0), c01b(0.0);
                SIMD<double, 2> c10a(0.0), c10b(0.0);
                SIMD<double, 2> c11a(0.0), c11b(0.0);

                size_t k = 0;
                // Unroll k by 4 -> we create two SIMD loads per A/B pair (k,k+1) and (k+2,k+3)
                for (; k + 3 < K; k += 4)
                {
                    // load two lanes [k,k+1] and [k+2,k+3]
                    SIMD<double, 2> a0_0(A_row0[k], A_row0[k + 1]);     // A(i,k..k+1)
                    SIMD<double, 2> a0_1(A_row0[k + 2], A_row0[k + 3]); // A(i,k+2..k+3)
                    SIMD<double, 2> a1_0(A_row1[k], A_row1[k + 1]);     // A(i+1,k..k+1)
                    SIMD<double, 2> a1_1(A_row1[k + 2], A_row1[k + 3]); // A(i+1,k+2..k+3)

                    SIMD<double, 2> b0_0(B_row0[k], B_row0[k + 1]);     // B_T(j,k..k+1)
                    SIMD<double, 2> b0_1(B_row0[k + 2], B_row0[k + 3]); // B_T(j,k+2..k+3)
                    SIMD<double, 2> b1_0(B_row1[k], B_row1[k + 1]);     // B_T(j+1,k..k+1)
                    SIMD<double, 2> b1_1(B_row1[k + 2], B_row1[k + 3]); // B_T(j+1,k+2..k+3)

                    // Pair of FMAs for lanes [k,k+1]
                    c00a = FMA(a0_0, b0_0, c00a);
                    c01a = FMA(a0_0, b1_0, c01a);
                    c10a = FMA(a1_0, b0_0, c10a);
                    c11a = FMA(a1_0, b1_0, c11a);

                    // Pair of FMAs for lanes [k+2,k+3]
                    c00b = FMA(a0_1, b0_1, c00b);
                    c01b = FMA(a0_1, b1_1, c01b);
                    c10b = FMA(a1_1, b0_1, c10b);
                    c11b = FMA(a1_1, b1_1, c11b);
                }

                // Handle remaining k in blocks of 2
                for (; k + 1 < K; k += 2)
                {
                    SIMD<double, 2> a0(A_row0[k], A_row0[k + 1]);
                    SIMD<double, 2> a1(A_row1[k], A_row1[k + 1]);
                    SIMD<double, 2> b0(B_row0[k], B_row0[k + 1]);
                    SIMD<double, 2> b1(B_row1[k], B_row1[k + 1]);

                    c00a = FMA(a0, b0, c00a);
                    c01a = FMA(a0, b1, c01a);
                    c10a = FMA(a1, b0, c10a);
                    c11a = FMA(a1, b1, c11a);
                }

                // Handle leftover single k (odd)
                if (k < K)
                {
                    double a0s = A_row0[k];
                    double a1s = A_row1[k];
                    double b0s = B_row0[k];
                    double b1s = B_row1[k];

                    c00a = c00a + SIMD<double, 2>(a0s * b0s);
                    c01a = c01a + SIMD<double, 2>(a0s * b1s);
                    c10a = c10a + SIMD<double, 2>(a1s * b0s);
                    c11a = c11a + SIMD<double, 2>(a1s * b1s);
                }

                // Reduce accumulators: sum 'a' + 'b' parts
                SIMD<double, 2> c00 = c00a + c00b;
                SIMD<double, 2> c01 = c01a + c01b;
                SIMD<double, 2> c10 = c10a + c10b;
                SIMD<double, 2> c11 = c11a + c11b;

                // Horizontal sums to scalars
                double s00 = HSum(c00);
                double s01 = HSum(c01);
                double s10 = HSum(c10);
                double s11 = HSum(c11);

                // Store back (accumulate)
                C_row0[j] += s00;
                C_row0[j + 1] += s01;
                C_row1[j] += s10;
                C_row1[j + 1] += s11;
            } // j loop

            // tail j if N odd
            for (; j < N; ++j)
            {
                const double *B_row = Bdata + j * ldb;
                double sum0 = 0.0, sum1 = 0.0;
                size_t k = 0;
                for (; k + 3 < K; k += 4)
                {
                    sum0 += A_row0[k] * B_row[k] + A_row0[k + 1] * B_row[k + 1] + A_row0[k + 2] * B_row[k + 2] + A_row0[k + 3] * B_row[k + 3];
                    sum1 += A_row1[k] * B_row[k] + A_row1[k + 1] * B_row[k + 1] + A_row1[k + 2] * B_row[k + 2] + A_row1[k + 3] * B_row[k + 3];
                }
                for (; k < K; ++k)
                {
                    sum0 += A_row0[k] * B_row[k];
                    sum1 += A_row1[k] * B_row[k];
                }
                C_row0[j] += sum0;
                C_row1[j] += sum1;
            }
        } // i loop

        // leftover last row if M odd
        for (; i < M; ++i)
        {
            double *C_row = Cdata + i * ldc;
            const double *A_row = Adata + i * lda;
            for (size_t j = 0; j < N; ++j)
            {
                const double *B_row = Bdata + j * ldb;
                double sum = 0.0;
                size_t k = 0;
                for (; k + 3 < K; k += 4)
                {
                    sum += A_row[k] * B_row[k] + A_row[k + 1] * B_row[k + 1] + A_row[k + 2] * B_row[k + 2] + A_row[k + 3] * B_row[k + 3];
                }
                for (; k < K; ++k)
                    sum += A_row[k] * B_row[k];
                C_row[j] += sum;
            }
        }
    }
    template <int UNROLL_K = 4, int ACCUMS = 4, bool PREFETCH = true,
              typename MA, typename MB, typename MC>
    inline void EvalMatMatMultiplyDouble_T3(const MA &A_pack_or_view, const MB &B_transposed, MC &C)
    {
        static_assert(UNROLL_K % 2 == 0, "UNROLL_K must be multiple of 2 for SIMD<double,2>");

        using namespace bla_ga;
        const size_t M = A_pack_or_view.nRows();
        const size_t K = A_pack_or_view.nCols(); // packed A is M_block x K_block
        const size_t N = B_transposed.nRows();   // packed: rows == original N_block

        // Data pointers and strides: packed A is row-major so Dist() == K
        double *Cdata = C.Data();
        const double *Adata = A_pack_or_view.Data();
        const double *Bdata = B_transposed.Data();

        const size_t lda = A_pack_or_view.Dist(); // should be == K
        const size_t ldb = B_transposed.Dist();   // should be == K
        const size_t ldc = C.Dist();

        // Work 2x2 tiles on C, as before
        size_t i = 0;
        for (; i + 1 < M; i += 2)
        {
            double *C_row0 = Cdata + i * ldc;
            double *C_row1 = Cdata + (i + 1) * ldc;

            const double *A_row0 = Adata + i * lda;
            const double *A_row1 = Adata + (i + 1) * lda;

            size_t j = 0;
            for (; j + 1 < N; j += 2)
            {
                const double *B_row0 = Bdata + j * ldb;       // B_transposed row j (K elems)
                const double *B_row1 = Bdata + (j + 1) * ldb; // B_transposed row j+1

                if constexpr (PREFETCH)
                {
                    // prefetch the beginning of the B rows (tweak offset if desired)
                    __builtin_prefetch(B_row0, 0, 3);
                    __builtin_prefetch(B_row1, 0, 3);
                }

                // Create ACCUMS independent accumulators per C element: e.g. c00a,c00b,...
                // We'll store them into arrays for simpler code
                SIMD<double, 2> c00a[ACCUMS], c01a[ACCUMS], c10a[ACCUMS], c11a[ACCUMS];
                for (int t = 0; t < ACCUMS; ++t)
                {
                    c00a[t] = SIMD<double, 2>(0.0);
                    c01a[t] = SIMD<double, 2>(0.0);
                    c10a[t] = SIMD<double, 2>(0.0);
                    c11a[t] = SIMD<double, 2>(0.0);
                }

                size_t k = 0;
                // Unroll by UNROLL_K (we load UNROLL_K values per lane in chunks of 2)
                for (; k + (UNROLL_K - 1) < K; k += UNROLL_K)
                {
                    // split UNROLL_K into pairs: process UNROLL_K/2 pairs
                    int pair_index = 0;
                    for (size_t kk = k; kk < k + UNROLL_K; kk += 2)
                    {
                        // lanes for this pair
                        SIMD<double, 2> a0(A_row0[kk], A_row0[kk + 1]);
                        SIMD<double, 2> a1(A_row1[kk], A_row1[kk + 1]);

                        SIMD<double, 2> b0(B_row0[kk], B_row0[kk + 1]);
                        SIMD<double, 2> b1(B_row1[kk], B_row1[kk + 1]);

                        // schedule into accumulator slot pair_index % ACCUMS
                        int slot = pair_index % ACCUMS;
                        c00a[slot] = FMA(a0, b0, c00a[slot]);
                        c01a[slot] = FMA(a0, b1, c01a[slot]);
                        c10a[slot] = FMA(a1, b0, c10a[slot]);
                        c11a[slot] = FMA(a1, b1, c11a[slot]);

                        ++pair_index;
                    }
                }

                // handle remaining k in pairs
                for (; k + 1 < K; k += 2)
                {
                    SIMD<double, 2> a0(A_row0[k], A_row0[k + 1]);
                    SIMD<double, 2> a1(A_row1[k], A_row1[k + 1]);
                    SIMD<double, 2> b0(B_row0[k], B_row0[k + 1]);
                    SIMD<double, 2> b1(B_row1[k], B_row1[k + 1]);

                    c00a[0] = FMA(a0, b0, c00a[0]);
                    c01a[0] = FMA(a0, b1, c01a[0]);
                    c10a[0] = FMA(a1, b0, c10a[0]);
                    c11a[0] = FMA(a1, b1, c11a[0]);
                }

                // leftover single k
                if (k < K)
                {
                    double a0s = A_row0[k];
                    double a1s = A_row1[k];
                    double b0s = B_row0[k];
                    double b1s = B_row1[k];
                    c00a[0] = c00a[0] + SIMD<double, 2>(a0s * b0s);
                    c01a[0] = c01a[0] + SIMD<double, 2>(a0s * b1s);
                    c10a[0] = c10a[0] + SIMD<double, 2>(a1s * b0s);
                    c11a[0] = c11a[0] + SIMD<double, 2>(a1s * b1s);
                }

                // Reduce accumulators across slots
                SIMD<double, 2> c00 = c00a[0], c01 = c01a[0], c10 = c10a[0], c11 = c11a[0];
                for (int t = 1; t < ACCUMS; ++t)
                {
                    c00 = c00 + c00a[t];
                    c01 = c01 + c01a[t];
                    c10 = c10 + c10a[t];
                    c11 = c11 + c11a[t];
                }

                // horizontal sums
                double s00 = HSum(c00);
                double s01 = HSum(c01);
                double s10 = HSum(c10);
                double s11 = HSum(c11);

                // accumulate into C
                C_row0[j] += s00;
                C_row0[j + 1] += s01;
                C_row1[j] += s10;
                C_row1[j + 1] += s11;
            } // end j loop

            // handle tail j columns (same scalar fallback as before, you can also vectorize similarly)
            for (; j < N; ++j)
            {
                const double *B_row = Bdata + j * ldb;
                double sum0 = 0.0, sum1 = 0.0;
                size_t k = 0;
                for (; k + 3 < K; k += 4)
                {
                    sum0 += A_row0[k] * B_row[k] + A_row0[k + 1] * B_row[k + 1] + A_row0[k + 2] * B_row[k + 2] + A_row0[k + 3] * B_row[k + 3];
                    sum1 += A_row1[k] * B_row[k] + A_row1[k + 1] * B_row[k + 1] + A_row1[k + 2] * B_row[k + 2] + A_row1[k + 3] * B_row[k + 3];
                }
                for (; k < K; ++k)
                {
                    sum0 += A_row0[k] * B_row[k];
                    sum1 += A_row1[k] * B_row[k];
                }
                C_row0[j] += sum0;
                C_row1[j] += sum1;
            }
        } // end i loop

        // leftover M row if odd (scalar fallback)
        for (; i < M; ++i)
        {
            double *C_row = Cdata + i * ldc;
            const double *A_row = Adata + i * lda;
            for (size_t j = 0; j < N; ++j)
            {
                const double *B_row = Bdata + j * ldb;
                double sum = 0.0;
                size_t k = 0;
                for (; k + 3 < K; k += 4)
                {
                    sum += A_row[k] * B_row[k] + A_row[k + 1] * B_row[k + 1] + A_row[k + 2] * B_row[k + 2] + A_row[k + 3] * B_row[k + 3];
                }
                for (; k < K; ++k)
                    sum += A_row[k] * B_row[k];
                C_row[j] += sum;
            }
        }
    }
    /*
        template <typename MA, typename MB, typename MC>
        inline void SIMDEvalMatMatMultiplyDouble(const MA &A, const MB &B, MC &C)
        {
            const size_t M = A.nRows();
            const size_t K = A.nCols();
            const size_t N = B.nCols();

            // Tuned block sizes for M1 cache (fits L1/L2 nicely)
            constexpr size_t BM = 64;
            constexpr size_t BK = 64;
            constexpr size_t BN = 64;

            for (size_t ii = 0; ii < M; ii += BM)
            {
                size_t i_max = std::min(M, ii + BM);
                for (size_t kk = 0; kk < K; kk += BK)
                {
                    size_t k_max = std::min(K, kk + BK);
                    // Sub-block of A
                    auto Ablock = A.Rows(ii, i_max).Cols(kk, k_max);

                    for (size_t jj = 0; jj < N; jj += BN)
                    {
                        size_t j_max = std::min(N, jj + BN);
                        // Sub-block of B (transposed for better locality)
                        auto Bblock = Matrix(B.Rows(kk, k_max).Cols(jj, j_max)).Transposed();
                        auto Cblock = C.Rows(ii, i_max).Cols(jj, j_max);

                        // Multiply the small blocks
                        EvalMatMatMultiplyDouble_T(Ablock, Bblock, Cblock);
                    }
                }
            }
        }

        // Same as before but with raw pointer arithmetic for inner speed
        template <typename MA, typename MB, typename MC>
        inline void EvalMatMatMultiplyDouble_T(const MA &A, const MB &B_T, MC &C)
        {
            const size_t M = A.nRows();
            const size_t K = A.nCols();
            const size_t N = B_T.nRows(); // B is transposed, so rows = original cols

            double *Cdata = C.Data();
            const double *Adata = A.Data();
            const double *Bdata = B_T.Data();

            const size_t lda = A.Dist();   // distance between cols in A
            const size_t ldb = B_T.Dist(); // distance between cols in B_T (i.e., rows in original B)
            const size_t ldc = C.Dist();

            for (size_t i = 0; i < M; ++i)
            {
                double *C_row = Cdata + i * ldc;
                const double *A_row = Adata + i * lda;

                for (size_t j = 0; j < N; ++j)
                {
                    const double *B_row = Bdata + j * ldb;
                    double sum = C_row[j]; // accumulate

                    // Inner loop – compiler auto-vectorizes this perfectly on M1
                    for (size_t k = 0; k < K; ++k)
                    {
                        sum += A_row[k] * B_row[k];
                    }

                    C_row[j] = sum;
                }
            }
        }
            */

} // namespace bla_ga

#endif // FILE_MAT_EXPRESSION_SPECIALIZATION
