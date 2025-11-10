#include <iostream>
#include <chrono>
#include "matrix.hpp"
#include "matrixexpression.hpp"
#include "vector.hpp"

using namespace bla_ga;

template <typename T = double>
void benchmark_matrix_matrix_multiplication(int n)
{
    Matrix<T> A(n, n);
    Matrix<T> B(n, n);
    Matrix<T> C(n, n);
    Matrix<T> C_ex(n, n);

    // Initialize matrices with deterministic values
    for (int j = 0; j < n; j++)
        for (int i = 0; i < n; i++)
        {
            A(i, j) = j;
            B(i, j) = i;
            C_ex(i, j) = 0.0;
            C(i, j) = 0.0;
        }
    auto start_ex = std::chrono::high_resolution_clock::now();
    EvalMatMatMultiplyDouble(A, B, C_ex);
    auto end_ex = std::chrono::high_resolution_clock::now();
    double elapsed_ex = std::chrono::duration<double>(end_ex - start_ex).count();

    double gflops_ex = 2.0 * n * n * n / (elapsed_ex * 1e9);

    std::cout << "Size: " << n << "x" << n

              << ", Time: " << elapsed_ex << " s"
              << ", GFLOPS: " << gflops_ex
              << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    SIMDEvalMatMatMultiplyDouble(A, B, C);

    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();
    // check if the result is correct
    bool correct = true;
    double tol = 1e-9;
    for (int j = 0; j < n; j++)
        for (int i = 0; i < n; i++)
            if (std::abs(C(i, j) - C_ex(i, j)) > tol)
                correct = false;

    double gflops = 2.0 * n * n * n / (elapsed * 1e9);

    std::cout << "Size: " << n << "x" << n
              << ", Time: " << elapsed << " s"
              << ", GFLOPS: " << gflops
              << std::endl;

    std::cout << (correct ? "Result OK" : "Result MISMATCH") << std::endl;
    // if missmatch print 3 x 3 matrices C and C_ex using Rows().Cols()
    if (!correct)
    {
        std::cout << "C = " << std::endl;
        std::cout << C.Rows(0, 3).Cols(0, 3) << std::endl;
        std::cout << "C_ex = " << std::endl;
        std::cout << C_ex.Rows(0, 3).Cols(0, 3) << std::endl;
    }
    return;
}

int main()
{
    int sizes[] = {1, 2, 4, 8, 16, 32, 64, 128, 255, 511, 1021, 2048, 3000};
    for (auto n : sizes)
        benchmark_matrix_matrix_multiplication<double>(n);
    return 0;
}
