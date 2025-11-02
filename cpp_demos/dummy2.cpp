#include <iostream>

#include <vector.hpp>

namespace bla = bla_ga;
void test_vector_range()
{
    size_t n = 2;
    bla_ga::Vector<double> x(n);
    for (size_t i = 0; i < n; i++)
    {
        x(i) = i;
    }

    // print n print x
    std::cout << "n = " << n << std::endl;
    std::cout << "x = " << x << std::endl;

    for (size_t end = 1; end < n + 1; end++)
    {
        for (size_t begin = 0; begin <= end; begin++)
        {
            bla_ga::Vector<double> y(end - begin);
            size_t counter = 0;
            for (size_t i = begin; i < end; i++)
            {
                y(counter) = i;
                counter = counter + 1;
            };
            std::cout << "Begin : " << begin << " , end : " << end << std::endl;
            std::cout << "y = " << y << std::endl;
            std::cout << "Range(begin, end)"
                      << x.Range(begin, end) << std::endl
                      << std::endl;

            // in case it fails show
        }
    }
}

int main()
{
    test_vector_range();
    return 0;
}
