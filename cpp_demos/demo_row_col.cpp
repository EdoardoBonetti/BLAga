#include <iostream>

#include <matrix.hpp>

namespace bla = bla_ga;

int main()
{
    size_t n = 10;

    bla::Matrix<double, bla::RowMajor> x(n - 3, n);

    for (size_t i = 0; i < x.nRows(); i++)
        for (size_t j = 0; j < x.nCols(); j++)
        {
            x(i, j) = j + 10 * i;
        }

    std::cout << "original matrix x = \n"
              << x << std::endl;
    // print out the Rows

    std::cout << "Printing Rows \n"
              << std::endl;
    for (size_t i = 0; i < x.nRows(); i++)
    {
        std::cout << x.Row(i) << std::endl;
    }

    std::cout << "Printing Cols \n"
              << std::endl;
    // print out the Cols
    for (size_t i = 0; i < x.nCols(); i++)
    {
        std::cout << x.Col(i) << std::endl;
    }

    // print Transpose:
    std::cout << "Transpose: " << x.Transpose() << std::endl;

    // test out Rows(begin, end)
    std::cout << "Rows(begin, end): " << x.Rows(2, 6) << std::endl;

    // test out Rows(begin, end)
    std::cout << "Cols(begin, end): " << x.Cols(2, 6) << std::endl;

    // pritnx x and info about x
    std::cout << "x = " << x << std::endl;
    std::cout << "x.nRows(): " << x.nRows() << std::endl;
    std::cout << "x.nCols(): " << x.nCols() << std::endl;
    std::cout << "x.Dist(): " << x.Dist() << std::endl;

    // info about x.Cols(2,6)
    std::cout << "Cols(begin, end): " << x.Cols(2, 6) << std::endl;
    std::cout << "Cols(begin, end).nRows(): " << x.Cols(2, 6).nRows() << std::endl;
    std::cout << "Cols(begin, end).nCols(): " << x.Cols(2, 6).nCols() << std::endl;
    std::cout << "Cols(begin, end).Dist(): " << x.Cols(2, 6).Dist() << std::endl;

    return 0;
}