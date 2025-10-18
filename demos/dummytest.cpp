#include <iostream>

#include <matrix.hpp>

namespace bla = bla_ga;

int main()
{
    size_t n = 10;
    size_t m = 10;

    bla::Matrix<double, bla::ColMajor> x(n, m);

    for (size_t i = 0; i < x.nRows(); i++)
        for (size_t j = 0; j < x.nCols(); j++)
        {
            x(i, j) = i * 10 + j;
            std::cout << x(i, j) << " ";
        }

    std::cout << "x= " << std::endl
              << x << std::endl;

    bla::Vector<double> y(x.Flatten());

    std::cout << "y= " << std::endl
              << y << std::endl;
}
