#include <iostream>
#include <chrono>
#include "matrix.hpp"
#include "vector.hpp"
#include "matrixexpression.hpp"
#include "vectorexpression.hpp"

using bla_ga::Matrix;
using bla_ga::RowMajor;

int main()
{
    // size_t N = 2 * 1023;
    size_t N = 2 * 1024;
    std::cout << "Matrix size: " << N << " x " << N << std::endl;
    Matrix<double, RowMajor> A(N, N);
    Matrix<double, RowMajor> B(N, N);
    Matrix<double, RowMajor> C(N, N, 0.0);

    for (int j = 0; j < N; j++)
        for (int i = 0; i < N; i++)
        {
            A(i, j) = 1;
            B(i, j) = 1;
        }

    auto start = std::chrono::high_resolution_clock::now();
    bla_ga::SIMDEvalMatMatMultiplyDouble(A, B, C);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << C(0, 0) << " " << C(N - 1, N - 1) << std::endl;

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

    double gflops = 2 * N * N * N / (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() * 1e6);
    std::cout << "GFLOPS: " << gflops << std::endl;
    return 0;
}
