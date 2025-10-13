#include <catch2/catch_test_macros.hpp>
#include "vector.hpp"

// include standard for std::vector
#include <valarray>
#include <complex>

using complex = std::complex<double>;
// use 1i as imaginary unit

using namespace bla_ga;

template <typename T, typename S>
void test_vector_add()
{
    int n = 100;

    Vector<T> v1(n);
    Vector<S> v2(n);

    using U = decltype(T{} + S{});

    std::valarray<U> w(n);

    for (int i = 0; i < n; i++)
    {
        v1(i) = T(i);      // cast i to T (works for int, double, complex)
        v2(i) = S(10 * i); // cast 10*i to S

        w[i] = T(i) + S(10 * i); // cast loop variables to match scalar types
    }

    Vector<U> vout = v1 + v2;

    for (int i = 0; i < n; i++)
    {
        REQUIRE(vout(i) == w[i]);
    }
}

template <typename T, typename S>
void test_vector_mult()
{
    int n = 100;

    T a = T(2);
    Vector<S> v(n);

    using U = decltype(T{} * S{});

    std::valarray<U> w(n);

    for (int i = 0; i < n; i++)
    {
        v(i) = T(i); // cast i to T (works for int, double, complex)

        w[i] = a * T(i); // cast loop variables to match scalar types
    }

    Vector<U> vout = a * v;

    for (int i = 0; i < n; i++)
    {
        REQUIRE(vout(i) == w[i]);
    }
}

TEST_CASE("Vector addition works for multiple types", "[math]")
{
    SECTION("4int") { test_vector_add<int, int>(); }
    SECTION("4double") { test_vector_add<double, double>(); }
    SECTION("4complex") { test_vector_add<complex, complex>(); }
    SECTION("2double2int") { test_vector_add<double, int>(); }
    SECTION("2complex2double") { test_vector_add<complex, double>(); }
    SECTION("2double2complex") { test_vector_add<double, complex>(); }
}

TEST_CASE("Vector multiplication works for multiple types", "[math]")
{
    SECTION("4int") { test_vector_mult<int, int>(); }
    SECTION("4double") { test_vector_mult<double, double>(); }
    SECTION("4complex") { test_vector_mult<complex, complex>(); }
    SECTION("2double2int") { test_vector_mult<double, int>(); }
}