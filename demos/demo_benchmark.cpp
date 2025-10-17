#include <iostream>
#include <chrono>
#include "matrix.hpp"
#include "vector.hpp"
#include "matrixexpression.hpp"
#include "vectorexpression.hpp"

using namespace bla_ga;

template <ORDERING ORDT, ORDERING ORDS>
void benchmark_matrix_matrix_multiplication(int iterations, int m, int k, int n)
{
    double vali = 2.0;
    double valj = 1.0;

    // First matrix (m x k)
    Matrix<double, ORDT> mat1(m, k);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < k; j++)
            mat1(i, j) = vali * double(i) + valj * double(j);

    // Second matrix (k x n)
    Matrix<double, ORDS> mat2(k, n);
    for (int i = 0; i < k; i++)
        for (int j = 0; j < n; j++)
            mat2(i, j) = 1.0;

    // Start timing
    auto start = std::chrono::high_resolution_clock::now();

    for (int iter = 0; iter < iterations; iter++)
    {
        // Force evaluation of the expression into a temporary
        auto tmp = mat1 * mat2;

        // Then assign to result
        Matrix<double, ORDT> result(mat1.nRows(), mat2.nCols());
        result = tmp;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Matrix multiplication ("
              << (ORDT == ColMajor ? "ColMajor" : "RowMajor") << " x "
              << (ORDS == ColMajor ? "ColMajor" : "RowMajor")
              << "), size: " << m << "x" << k << " * " << k << "x" << n
              << ", iterations: " << iterations
              << ", elapsed time: " << elapsed.count() << " s"
              << ", avg per multiplication: " << (elapsed.count() / iterations) << " s\n";
}

int main()
{

    // Benchmark small matrices (more iterations)
    benchmark_matrix_matrix_multiplication<ColMajor, ColMajor>(1000, 5, 4, 6);
    benchmark_matrix_matrix_multiplication<RowMajor, RowMajor>(1000, 5, 4, 6);
    benchmark_matrix_matrix_multiplication<ColMajor, RowMajor>(1000, 5, 4, 6);
    benchmark_matrix_matrix_multiplication<RowMajor, ColMajor>(1000, 5, 4, 6);

    // Benchmark medium matrices (fewer iterations)
    benchmark_matrix_matrix_multiplication<ColMajor, ColMajor>(500, 50, 50, 50);
    benchmark_matrix_matrix_multiplication<RowMajor, RowMajor>(500, 50, 50, 50);
    benchmark_matrix_matrix_multiplication<ColMajor, RowMajor>(500, 50, 50, 50);
    benchmark_matrix_matrix_multiplication<RowMajor, ColMajor>(500, 50, 50, 50);

    // Benchmark large matrices (even fewer iterations)
    benchmark_matrix_matrix_multiplication<ColMajor, ColMajor>(100, 200, 200, 200);
    benchmark_matrix_matrix_multiplication<RowMajor, RowMajor>(100, 200, 200, 200);
    benchmark_matrix_matrix_multiplication<ColMajor, RowMajor>(100, 200, 200, 200);
    benchmark_matrix_matrix_multiplication<RowMajor, ColMajor>(100, 200, 200, 200);

    // Benchmark large matrices (even fewer iterations)
    benchmark_matrix_matrix_multiplication<ColMajor, ColMajor>(100, 2000, 2000, 2000);
    benchmark_matrix_matrix_multiplication<RowMajor, RowMajor>(100, 2000, 2000, 2000);
    benchmark_matrix_matrix_multiplication<ColMajor, RowMajor>(100, 2000, 2000, 2000);
    benchmark_matrix_matrix_multiplication<RowMajor, ColMajor>(100, 2000, 2000, 2000);

    return 0;
}
