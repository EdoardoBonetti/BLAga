/*
TODO : Test cases for mixed copy constructor

*/
#include <catch2/catch_test_macros.hpp>

#include "matrix.hpp"

// include standard for std::vector
#include <valarray>
#include <complex>

using complex = std::complex<double>;
// use 1i as imaginary unit

using namespace bla_ga;

template <typename T, ORDERING ORD>
void test_matrix_constructor()
{
    int m = 100;
    int n = 100;

    T vali = T(10.5);
    T valj = T(1.5);

    Matrix<T, ORD> mat(m, n);

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            mat(i, j) = vali * T(i) + valj * T(j);
        }
    }

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            REQUIRE(mat(i, j) == vali * T(i) + valj * T(j));
        }
    }
}

TEST_CASE("test_matrix_constructor", "[matrix]")
{
    SECTION("int ColMajor") { test_matrix_constructor<int, ColMajor>(); }
    SECTION("int RowMajor") { test_matrix_constructor<int, RowMajor>(); }
    SECTION("double ColMajor") { test_matrix_constructor<double, ColMajor>(); }
    SECTION("double RowMajor") { test_matrix_constructor<double, RowMajor>(); }
    SECTION("complex ColMajor") { test_matrix_constructor<complex, ColMajor>(); }
    SECTION("complex RowMajor") { test_matrix_constructor<complex, RowMajor>(); }
}

template <typename T, ORDERING ORD>
void test_matrix_move_constructor()
{
    int m = 30;
    int n = 10;

    T vali = T(10.5);
    T valj = T(1.5);

    // Original matrix
    Matrix<T, ORD> mat(m, n);

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            mat(i, j) = vali * T(i) + valj * T(j);

    // Move constructor
    Matrix<T, ORD> moved_mat(std::move(mat));

    // Verify moved matrix has correct values
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            REQUIRE(moved_mat(i, j) == vali * T(i) + valj * T(j));

    // Optional: Verify original matrix is now in a valid but unspecified state
    // For example, size might be 0
}

TEST_CASE("test_matrix_move_constructor", "[matrix]")
{
    SECTION("int ColMajor") { test_matrix_move_constructor<int, ColMajor>(); }
    SECTION("int RowMajor") { test_matrix_move_constructor<int, RowMajor>(); }
    SECTION("double ColMajor") { test_matrix_move_constructor<double, ColMajor>(); }
    SECTION("double RowMajor") { test_matrix_move_constructor<double, RowMajor>(); }
    SECTION("complex ColMajor") { test_matrix_move_constructor<complex, ColMajor>(); }
    SECTION("complex RowMajor") { test_matrix_move_constructor<complex, RowMajor>(); }
}

template <typename T, ORDERING ORD>
void test_matrix_sum()
{
    int m = 20;
    int n = 15;

    T vali = T(2.0);
    T valj = T(3.0);

    // First matrix
    Matrix<T, ORD> mat1(m, n);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            mat1(i, j) = vali * T(i) + valj * T(j);

    // Second matrix
    Matrix<T, ORD> mat2(m, n);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            mat2(i, j) = T(1); // simple constant matrix

    // Sum matrix
    Matrix<T, ORD> sum = mat1 + mat2;

    // Verify element-wise sum
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            REQUIRE(sum(i, j) == mat1(i, j) + mat2(i, j));
}

TEST_CASE("test_matrix_sum", "[matrix]")
{
    SECTION("int ColMajor") { test_matrix_sum<int, bla_ga::ColMajor>(); }
    SECTION("int RowMajor") { test_matrix_sum<int, bla_ga::RowMajor>(); }
    SECTION("double ColMajor") { test_matrix_sum<double, bla_ga::ColMajor>(); }
    SECTION("double RowMajor") { test_matrix_sum<double, bla_ga::RowMajor>(); }
    SECTION("complex ColMajor") { test_matrix_sum<std::complex<double>, bla_ga::ColMajor>(); }
    SECTION("complex RowMajor") { test_matrix_sum<std::complex<double>, bla_ga::RowMajor>(); }
}

template <typename T, ORDERING ORD, typename S>
void test_matrix_scalar_multiplication()
{
    int m = 20;
    int n = 15;

    T vali = T(2.0);
    T valj = T(3.0);
    S scalar = S(5); // scalar to multiply with

    // Original matrix
    bla_ga::Matrix<T, ORD> mat(m, n);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            mat(i, j) = vali * T(i) + valj * T(j);

    // Multiply by scalar
    auto result = scalar * mat;

    // Verify element-wise multiplication
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            REQUIRE(result(i, j) == mat(i, j) * scalar);
}

TEST_CASE("test_matrix_scalar_multiplication", "[matrix]")
{
    SECTION("int ColMajor int") { test_matrix_scalar_multiplication<int, bla_ga::ColMajor, int>(); }
    SECTION("int RowMajor int") { test_matrix_scalar_multiplication<int, bla_ga::RowMajor, int>(); }
    SECTION("double ColMajor double") { test_matrix_scalar_multiplication<double, bla_ga::ColMajor, double>(); }
    SECTION("double RowMajor double") { test_matrix_scalar_multiplication<double, bla_ga::RowMajor, double>(); }
    SECTION("complex ColMajor double") { test_matrix_scalar_multiplication<std::complex<double>, bla_ga::ColMajor, double>(); }
    SECTION("complex RowMajor double") { test_matrix_scalar_multiplication<std::complex<double>, bla_ga::RowMajor, double>(); }
}

template <typename T, ORDERING ORDT, typename S, ORDERING ORDS>
void test_matrix_matrix_multiplication()
{
    int m = 5; // rows of first matrix
    int k = 4; // cols of first matrix / rows of second
    int n = 6; // cols of second matrix

    T vali = T(2.0);
    T valj = T(1.0);

    // First matrix (m x k)
    bla_ga::Matrix<T, ORDT> mat1(m, k);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < k; j++)
            mat1(i, j) = vali * T(i) + valj * T(j);

    // Second matrix (k x n)
    bla_ga::Matrix<S, ORDS> mat2(k, n);
    for (int i = 0; i < k; i++)
        for (int j = 0; j < n; j++)
            mat2(i, j) = S(1); // simple constant for easy verification

    // Multiply
    auto result = mat1 * mat2; // result is m x n

    // Verify multiplication manually
    using U = decltype(T{} * S{});
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            U expected = U(0);
            for (int t = 0; t < k; t++)
                expected += mat1(i, t) * mat2(t, j);
            REQUIRE(result(i, j) == expected);
        }
    }
}

TEST_CASE("test_matrix_matrix_multiplication", "[matrix]")
{
    SECTION("int ColMajor x int ColMajor") { test_matrix_matrix_multiplication<int, bla_ga::ColMajor, int, bla_ga::ColMajor>(); }
    SECTION("int RowMajor x int RowMajor") { test_matrix_matrix_multiplication<int, bla_ga::RowMajor, int, bla_ga::RowMajor>(); }
    SECTION("double ColMajor x double RowMajor") { test_matrix_matrix_multiplication<double, bla_ga::ColMajor, double, bla_ga::RowMajor>(); }
    SECTION("double RowMajor x double ColMajor") { test_matrix_matrix_multiplication<double, bla_ga::RowMajor, double, bla_ga::ColMajor>(); }
    SECTION("complex ColMajor x double RowMajor") { test_matrix_matrix_multiplication<std::complex<double>, bla_ga::ColMajor, double, bla_ga::RowMajor>(); }
    SECTION("complex RowMajor x double ColMajor") { test_matrix_matrix_multiplication<std::complex<double>, bla_ga::RowMajor, double, bla_ga::ColMajor>(); }
}

template <typename T, ORDERING ORD>
void test_matrix_flatten()
{
    int m = 4;
    int n = 3;

    T vali = T(1.5);
    T valj = T(2.0);

    // Create matrix
    bla_ga::Matrix<T, ORD> mat(m, n);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            mat(i, j) = vali * T(i) + valj * T(j);

    // Flatten
    auto flat = mat.Flatten();

    // Verify size
    REQUIRE(flat.Size() == m * n);

    // Verify elements
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            size_t idx = (ORD == bla_ga::RowMajor) ? i * n + j : j * m + i;
            REQUIRE(flat(idx) == mat(i, j));
        }
    }
}

TEST_CASE("test_matrix_flatten", "[matrix]")
{
    SECTION("int ColMajor") { test_matrix_flatten<int, bla_ga::ColMajor>(); }
    SECTION("int RowMajor") { test_matrix_flatten<int, bla_ga::RowMajor>(); }
    SECTION("double ColMajor") { test_matrix_flatten<double, bla_ga::ColMajor>(); }
    SECTION("double RowMajor") { test_matrix_flatten<double, bla_ga::RowMajor>(); }
    SECTION("complex ColMajor") { test_matrix_flatten<std::complex<double>, bla_ga::ColMajor>(); }
    SECTION("complex RowMajor") { test_matrix_flatten<std::complex<double>, bla_ga::RowMajor>(); }
}
template <typename T, ORDERING ORD>
void test_matrix_transpose()
{
    int m = 3;
    int n = 4;

    T vali = T(1.5);
    T valj = T(2.0);

    // Create original matrix
    bla_ga::Matrix<T, ORD> mat(m, n);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            mat(i, j) = vali * T(i) + valj * T(j);

    // Transpose
    auto trans = mat.Transpose();

    // Verify dimensions
    REQUIRE(trans.nRows() == mat.nCols());
    REQUIRE(trans.nCols() == mat.nRows());

    // Verify elements
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            REQUIRE(trans(j, i) == mat(i, j));
}

TEST_CASE("test_matrix_transpose", "[matrix]")
{
    SECTION("int ColMajor") { test_matrix_transpose<int, bla_ga::ColMajor>(); }
    SECTION("int RowMajor") { test_matrix_transpose<int, bla_ga::RowMajor>(); }
    SECTION("double ColMajor") { test_matrix_transpose<double, bla_ga::ColMajor>(); }
    SECTION("double RowMajor") { test_matrix_transpose<double, bla_ga::RowMajor>(); }
    SECTION("complex ColMajor") { test_matrix_transpose<complex, bla_ga::ColMajor>(); }
    SECTION("complex RowMajor") { test_matrix_transpose<complex, bla_ga::RowMajor>(); }
}

// test default matrix

void test_default_templates()
{
    // / I want to test the default templates as <double, ColMajor>
    Matrix<> mat1(10, 10);
    Matrix<double> mat2(10, 10);
    Matrix<double, ColMajor> mat3(10, 10);

    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
        {
            mat1(i, j) = double(i) + 10.0 * double(j);
            mat2(i, j) = double(i) + 10.0 * double(j);
        }

    Vector<double> vec1 = mat1.Flatten();
    Vector<double> vec2 = mat2.Flatten();
    Vector<double> vec3 = mat3.Flatten();

    // check if they are the same
    REQUIRE(vec1 == vec2);
    REQUIRE(vec2 == vec3);
    REQUIRE(vec3 == vec1);
}
