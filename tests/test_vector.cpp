#include <catch2/catch_test_macros.hpp>
#include "vector.hpp"

// include standard for std::vector
#include <valarray>
#include <complex>

using complex = std::complex<double>;
// use 1i as imaginary unit

using namespace bla_ga;

template <typename Tscal, typename T, typename Scal, typename S>
void test_vector_multaddmult()
{
    int n = 100;

    Vector<T> v1(n);
    Vector<S> v2(n);

    Tscal alpha1 = 3.;
    Scal alpha2 = 5.;

    using U = decltype(Tscal{} * T{} + Scal{} * S{});

    std::valarray<U> w(n);

    for (int i = 0; i < n; i++)
    {
        v1(i) = T(i);      // cast i to T (works for int, double, complex)
        v2(i) = S(10 * i); // cast 10*i to S

        w[i] = alpha1 * T(i) + alpha2 * S(10 * i); // cast loop variables to match scalar types
    }

    Vector<U> vout = alpha1 * v1 + alpha2 * v2;

    for (int i = 0; i < n; i++)
    {
        REQUIRE(vout(i) == w[i]);
    }
}

TEST_CASE("Vector addition works for multiple types", "[math]")
{
    SECTION("4int") { test_vector_multaddmult<int, int, int, int>(); }
    SECTION("4double") { test_vector_multaddmult<double, double, double, double>(); }
    SECTION("4complex") { test_vector_multaddmult<complex, complex, complex, complex>(); }
    SECTION("2double2int") { test_vector_multaddmult<double, double, int, int>(); }
    SECTION("2complex2double") { test_vector_multaddmult<complex, complex, double, double>(); }
    SECTION("2double2complex") { test_vector_multaddmult<double, double, complex, complex>(); }

    /*Other couples are not possible/reasonable */
}
