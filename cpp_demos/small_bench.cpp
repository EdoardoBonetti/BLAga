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
    size_t N = 1024;
    Matrix<double, RowMajor> A(N, N);
    Matrix<double, RowMajor> B(N, N);

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            A(i, j) = 1;
            B(i, j) = 2;
        }

    auto start = std::chrono::high_resolution_clock::now();
    Matrix<double, RowMajor> C(A * B);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << C(0, 0) << " " << C(N - 1, N - 1) << std::endl;

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
    return 0;
}
