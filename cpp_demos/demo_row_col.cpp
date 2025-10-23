#include <iostream>

#include <matrix.hpp>

namespace bla = bla_ga;

int main()
{
    size_t n = 10;
    bla::Matrix<double, bla::RowMajor> x(n, n);

    for (size_t i = 0; i < x.nRows(); i++)
        for (size_t j = 0; j < x.nCols(); j++)
        {
            x(i, j) = j + 10 * i;
        }
    std::cout << "x = " << x << std::endl;
    // print out the Rows
    for (size_t i = 0; i < x.nRows(); i++)
    {
        std::cout << x.Row(i) << std::endl;
    }

    // print out the Cols
    //    for (size_t i = 0; i < x.nCols(); i++)
    //    {
    //        std::cout << x.Col(i) << std::endl;
    //    }

    return 0;
}