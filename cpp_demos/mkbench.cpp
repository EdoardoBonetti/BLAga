#include <chrono>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <iomanip>

// Include your SIMD + microkernel headers here
#include "matrixexpressionspecialization.hpp"
using namespace bla_ga;

// =============================================================
// Your micro-kernel
// =============================================================
// template <size_t Hm, size_t Wm, size_t Lm>
// inline __attribute__((always_inline)) static void EvalMatMatMultiplyDouble_Micro(
//    const double *pntA, size_t ldA,
//    const double *pntBT, size_t ldBT,
//    double *pntC, size_t ldC)
//{
//    constexpr size_t Step = 8;
//
//    for (size_t i = 0; i < Hm; ++i)
//    {
//        const double *rowA = pntA + i * ldA;
//        double *rowC = pntC + i * ldC;
//
//        for (size_t j = 0; j < Wm; ++j)
//        {
//            const double *colB = pntBT + j * ldBT;
//            SIMD<double, 2> acc1(0.0), acc2(0.0), acc3(0.0), acc4(0.0);
//
//            for (size_t k = 0; k < Lm; k += Step)
//            {
//                acc1 = FMA(SIMD<double, 2>(&rowA[k + 0]), SIMD<double, 2>(&colB[k + 0]), acc1);
//                acc2 = FMA(SIMD<double, 2>(&rowA[k + 2]), SIMD<double, 2>(&colB[k + 2]), acc2);
//                acc3 = FMA(SIMD<double, 2>(&rowA[k + 4]), SIMD<double, 2>(&colB[k + 4]), acc3);
//                acc4 = FMA(SIMD<double, 2>(&rowA[k + 6]), SIMD<double, 2>(&colB[k + 6]), acc4);
//            }
//
//            double sum = HSum(acc1 + acc2 + acc3 + acc4);
//            rowC[j] += sum;
//        }
//    }
//}
//
//// =============================================================
// Benchmarking Harness
// =============================================================
template <size_t Hm, size_t Wm, size_t Lm>
double BenchmarkKernel(size_t repeats = 10000)
{
    constexpr size_t ldA = Lm;
    constexpr size_t ldBT = Lm;
    constexpr size_t ldC = Wm;

    // Allocate aligned memory
    std::vector<double> A(Hm * ldA);
    std::vector<double> BT(Wm * ldBT);
    std::vector<double> C(Hm * ldC, 0.0);

    // Fill with random values
    std::mt19937_64 rng(42);
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    for (auto &x : A)
        x = dist(rng);
    for (auto &x : BT)
        x = dist(rng);

    // Warm-up
    // for (int i = 0; i < 1; ++i)
    asm volatile("// === BEGIN MICROKERNEL ===");
    EvalMatMatMultiplyDouble_Micro<Hm, Wm, Lm>(A.data(), ldA, BT.data(), ldBT, C.data(), ldC);
    asm volatile("// === END MICROKERNEL ===");

    // Timed runs
    // auto start = std::chrono::high_resolution_clock::now();
    // for (size_t i = 0; i < repeats; ++i)
    //    EvalMatMatMultiplyDouble_Micro<Hm, Wm, Lm>(A.data(), ldA, BT.data(), ldBT, C.data(), ldC);
    // auto end = std::chrono::high_resolution_clock::now();

    // double elapsed = std::chrono::duration<double>(end - start).count();
    // double flops = double(Hm) * Wm * (2.0 * Lm) * repeats;
    return 1; // flops / elapsed / 1e9; // GFLOPS
}

// =============================================================
// Main: sweep over tile sizes
// =============================================================
int main()
{
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Testing microkernel GFLOPS (ARM64)...\n";
    std::cout << " Hm   Wm   Lm    GFLOPS\n";
    std::cout << "--------------------------\n";

    // Try different microkernel sizes
    constexpr size_t Lvals[] = {8};
    constexpr size_t Hvals[] = {4};
    constexpr size_t Wvals[] = {4};

    double best_gflops = 0.0;
    size_t best_H = 0, best_W = 0, best_L = 0;

    double gflops = 0.0;

    gflops = BenchmarkKernel<4, 4, 8>();

    return 0;
}
