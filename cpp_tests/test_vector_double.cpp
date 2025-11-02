// In this file we test for the vector double:
#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include "vector.hpp"

// create a vector x and y that go from 1 to n and from n to 1, add them:
using namespace bla_ga;
void test_vector_add()
{
    size_t n = 10;

    Vector<double> x(n);
    Vector<double> y(n);

    for (int i = 0; i < n; i++)
    {
        x(i) = i + 1;
        y(i) = n - i;
    }

    Vector<double> z = x + y;

    for (int i = 0; i < n; i++)
    {
        REQUIRE(z(i) == n + 1);
    }
}

TEST_CASE("Vector addition works", "[math]")
{
    test_vector_add();
}

// create a test case for the Range

void test_vector_range()
{
    size_t n = 13;
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

            // in case it fails show
            for (int i = 0; i < end - begin; i++)
            {
                REQUIRE(y(i) == x.Range(begin, end)(i));
            }
        }
    }
}

TEST_CASE("double Vector range works", "[math]")
{
    test_vector_range();
}

void test_vector_slice()
{
    size_t n = 13; // small size for easy debugging
    bla_ga::Vector<double> x(n);
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

            bla_ga::Vector<double> y(len);
            int counter = 0;

            for (size_t i = first; i < n; i += slice)
            {
                y(counter) = i;
                counter++;
            }

            // in case it fails show
            for (int i = 0; i < len; i++)
            {
                REQUIRE(y(i) == x.Slice(first, slice)(i));
            }
        }
    }
}

TEST_CASE("double Vector slice works", "[math]")
{
    test_vector_slice();
}

void test_vector_subvector()
{
    size_t n = 13;
    bla_ga::Vector<double> x(n);
    for (size_t i = 0; i < n; i++)
    {
        x(i) = i;
    }

    std::cout << "n = " << n << std::endl;
    std::cout << "x = " << x << std::endl
              << std::endl;

    // iterate over combinations
    for (size_t first = 0; first < n; first++)
    {
        for (size_t next = first + 1; next <= n; next++)
        {
            for (size_t stride = 1; stride <= (next - first); stride++)
            {
                // compute expected manually
                std::vector<double> expected;
                for (size_t i = first; i < next; i += stride)
                {
                    expected.push_back(x(i));
                }

                auto s = x.SubVector(first, next, stride);

                REQUIRE(s.Size() == expected.size());

                for (size_t i = 0; i < expected.size(); i++)
                {
                    REQUIRE(s(i) == expected[i]);
                }
            }
        }
    }
}

TEST_CASE("double Vector subvector works", "[math]")
{
    test_vector_subvector();
}
