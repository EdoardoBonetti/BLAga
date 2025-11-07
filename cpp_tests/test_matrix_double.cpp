#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "matrix.hpp"

using namespace bla_ga;
using Catch::Approx; // ✅ add this line

// void test_matrix_add()
//{
//     size_t rows = 4;
//     size_t cols = 5;
//
//     Matrix<double> A(rows, cols);
//     Matrix<double> B(rows, cols);
//
//     // Fill A(i,j) = i + j, B(i,j) = i - j
//     for (size_t i = 0; i < rows; i++)
//     {
//         for (size_t j = 0; j < cols; j++)
//         {
//             A(i, j) = static_cast<double>(i + j);
//             B(i, j) = static_cast<double>(i - j);
//         }
//     }
//
//     Matrix<double> C = A + B;
//
//     // A(i,j) + B(i,j) = (i+j) + (i-j) = 2*i
//     for (size_t i = 0; i < rows; i++)
//     {
//         for (size_t j = 0; j < cols; j++)
//         {
//             REQUIRE(C(i, j) == Approx(2.0 * i));
//         }
//     }
// }
//
// TEST_CASE("Matrix addition works", "[math]")
//{
//     test_matrix_add();
// }

// -------------------------------------------------------
/*
void test_matrix_scalar_multiplication()
{
    size_t rows = 3;
    size_t cols = 4;
    double a = 2.5;

    Matrix<double> A(rows, cols);
    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < cols; j++)
        {
            A(i, j) = i + j;
        }
    }

    Matrix<double> L = a * A;
    Matrix<double> R = A * a;

    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < cols; j++)
        {
            REQUIRE(L(i, j) == Approx(a * (i + j)));
            REQUIRE(R(i, j) == Approx(a * (i + j)));
        }
    }
}

TEST_CASE("Matrix scalar multiplication works", "[math]")
{
    test_matrix_scalar_multiplication();
}

// -------------------------------------------------------

void test_matrix_col_and_row()
{
    size_t rows = 5;
    size_t cols = 4;

    Matrix<double> A(rows, cols);
    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < cols; j++)
        {
            A(i, j) = i * 10 + j;
        }
    }

    // Check columns
    for (size_t j = 0; j < cols; j++)
    {
        Vector<double> col = A.Col(j);
        for (size_t i = 0; i < rows; i++)
        {
            REQUIRE(col(i) == Approx(A(i, j)));
        }
    }

    // Check rows
    for (size_t i = 0; i < rows; i++)
    {
        Vector<double> row = A.Row(i);
        for (size_t j = 0; j < cols; j++)
        {
            REQUIRE(row(j) == Approx(A(i, j)));
        }
    }
}

TEST_CASE("Matrix Row() and Col() work", "[math]")
{
    test_matrix_col_and_row();
}

// -------------------------------------------------------

void test_matrix_rows_and_cols_range()
{
    size_t rows = 6;
    size_t cols = 5;

    Matrix<double> A(rows, cols);
    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < cols; j++)
        {
            A(i, j) = i * 100 + j;
        }
    }

    // Test Cols(begin, end)
    for (size_t begin = 0; begin < cols; begin++)
    {
        for (size_t end = begin + 1; end <= cols; end++)
        {
            Matrix<double> sub = A.Cols(begin, end);
            REQUIRE(sub.Rows() == rows);
            REQUIRE(sub.Cols() == end - begin);

            for (size_t i = 0; i < rows; i++)
            {
                for (size_t j = begin; j < end; j++)
                {
                    REQUIRE(sub(i, j - begin) == Approx(A(i, j)));
                }
            }
        }
    }

    // Test Rows(begin, end)
    for (size_t begin = 0; begin < rows; begin++)
    {
        for (size_t end = begin + 1; end <= rows; end++)
        {
            Matrix<double> sub = A.Rows(begin, end);
            REQUIRE(sub.Rows() == end - begin);
            REQUIRE(sub.Cols() == cols);

            for (size_t i = begin; i < end; i++)
            {
                for (size_t j = 0; j < cols; j++)
                {
                    REQUIRE(sub(i - begin, j) == Approx(A(i, j)));
                }
            }
        }
    }
}

TEST_CASE("Matrix Rows() and Cols() ranges work", "[math]")
{
    test_matrix_rows_and_cols_range();
}
*/

//