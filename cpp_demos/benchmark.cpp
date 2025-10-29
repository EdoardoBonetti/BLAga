#include <iostream>
#include <chrono>
#include "matrix.hpp"
#include "matrixexpression.hpp"
#include "vector.hpp"

using namespace bla_ga;

template <typename T = double, ORDERING ORDT = RowMajor, ORDERING ORDS = RowMajor>
void benchmark_matrix_matrix_multiplication(int n, int iterations)
{
    Matrix<T, ORDT> A(n, n);
    Matrix<T, ORDS> B(n, n);
    Matrix<T, ORDT> C(n, n);

    // Initialize matrices with deterministic values
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            A(i, j) = 1.0 + i + j;
            B(i, j) = 1.0 / (1.0 + i + j);
        }

    auto start = std::chrono::high_resolution_clock::now();

    for (int iter = 0; iter < iterations; iter++)
    {
        // Evaluate and assign into C
        // C *= 0.0; // reset all elements

        C = A * B;
    }

    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();

    double gflops = 2.0 * n * n * n * iterations / (elapsed * 1e9);

    std::cout << "Size: " << n << "x" << n
              << ", Iterations: " << iterations
              << ", Time: " << elapsed / iterations << " s"
              << ", GFLOPS: " << gflops
              << std::endl;

    // To avoid optimization removal
    std::cout << "C(0,0) = " << C(0, 0) << std::endl;
}

int main()
{
    int sizes[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
    for (auto n : sizes)
        benchmark_matrix_matrix_multiplication<double, RowMajor, RowMajor>(n, 1);
    return 0;
}
