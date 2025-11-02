#include <iostream>

#include <matrix.hpp>

namespace bla = bla_ga;

void test_vector_range()
{
    size_t n = 2;
    bla_ga::Vector<double> x(n);
    for (int i = 0; i < n; i++)
    {
        x(i) = i;
    }

    for (size_t begin = 0; begin < n - 1; begin++)
    {
        for (size_t end = begin + 1; begin < n; end++)
        {
            bla_ga::Vector<double> y(end - begin);
            size_t counter = 0;
            for (size_t i = begin; i < end; i++)
            {
                y(counter) = i;
                counter = counter + 1;
            };
            std::cout << "Begin : " << begin << " , end : " << end << "\n";
            std::cout << "y = " << y;
            std::cout << "x.Range(begin, end)" << x.Range(begin, end) << "\n";

            // in case it fails show
        }
    }
}
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

    test_vector_range();
}
