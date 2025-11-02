#include <iostream>
#include "vector.hpp"

namespace bla = bla_ga;

void test_vector_slice()
{
    size_t n = 4; // small size for easy debugging
    bla::Vector<double> x(n);
    for (size_t i = 0; i < n; i++)
    {
        x(i) = i;
    }

    // print n, print x
    std::cout << "n = " << n << std::endl;
    std::cout << "x = " << x << std::endl
              << std::endl;

    // test different slice combinations

    for (size_t first = 0; first < n; first++)
    {
        for (size_t slice = 1; slice <= n - first; slice++)
        {

            // for loop to see how many elements we can reach:
            int len = 0;
            size_t i = first;
            for (; i < n; i += slice)
            {
                len++;
            }

            bla::Vector<double> y(len);
            int counter = 0;

            for (size_t i = first; i < n; i += slice)
            {
                y(counter) = i;
                counter++;
            }

            // get actual slice
            auto s = x.Slice(first, slice);
            // print : 1 + (size - 1 - first) / slice
            std::cout << "1 + (size - 1 - first) / slice = " << 1 + (n - 1 - first) / slice << std::endl;

            // print debug info
            std::cout << "first = " << first << " , slice = " << slice << std::endl;
            std::cout << "y (expected) = " << y << std::endl;
            std::cout << "x.Slice(first, slice) = s = " << s << std::endl;
            std::cout << "s.Size = " << s.Size() << std::endl
                      << "s.Dist = " << s.Dist() << std::endl
                      << std::endl;
        }
    }
}

int main()
{
    test_vector_slice();
    return 0;
}
