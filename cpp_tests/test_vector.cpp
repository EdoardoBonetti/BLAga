#include <catch2/catch_test_macros.hpp>

#include "vector.hpp"

// include standard for std::vector
#include <valarray>
#include <complex>

using complex = std::complex<double>;
// use 1i as imaginary unit

using namespace bla_ga;

template <typename T>
void test_vector_constructor()
{
    int n = 100;

    Vector<T> v(n);

    for (int i = 0; i < n; i++)
    {
        v(i) = T(10.5 * i);
    }

    for (int i = 0; i < n; i++)
    {
        REQUIRE(v(i) == T(10.5 * i));
    }
}

template <typename T>
void test_vector_copy_constructor()
{
    int n = 100;

    Vector<T> vin(n);

    for (int i = 0; i < n; i++)
    {
        vin(i) = T(10.5 * i);
    }

    Vector<T> vout(vin);

    for (int i = 0; i < n; i++)
    {
        REQUIRE(vout(i) == T(10.5 * i));
    }

    vout(0) = T(1000);

    REQUIRE(vout(0) == T(1000));
    REQUIRE(vin(0) == T(10.5 * 0));
}

template <typename T>
void test_vector_move_constructor()
{
    int n = 100;

    Vector<T> vin(n);

    for (int i = 0; i < n; i++)
    {
        vin(i) = T(10.5 * i);
    }

    Vector<T> vout(std::move(vin));
    for (int i = 0; i < n; i++)
    {
        REQUIRE(vout(i) == T(10.5 * i));
    }

    REQUIRE(vin.Size() == 0);
}

template <typename T>
void test_vector_destructor()
{
    {
        int n = 100;
        Vector<T> v1(n);
        for (int i = 0; i < n; ++i)
            v1(i) = T(i);

        Vector<T> v2(v1); // invokes copy constructor
        Vector<T> v3(n);
        v3 = v2; // invokes copy assignment
    } // <- all destructors run here
    REQUIRE(true); // Should complete without crashes or memory errors
}

template <typename T>
void test_vector_copy_assignment()
{
    int n = 100;

    Vector<T> v1(n);
    Vector<T> v2(n);

    for (int i = 0; i < n; i++)
    {
        v1(i) = T(10.5 * i);
        v2(i) = T(0);
    }

    v2 = v1; // invoke copy assignment operator

    for (int i = 0; i < n; i++)
    {
        REQUIRE(v2(i) == T(10.5 * i));
    }

    // Ensure v1 and v2 are independent after assignment
    v2(0) = T(999);
    REQUIRE(v2(0) == T(999));
    REQUIRE(v1(0) == T(10.5 * 0));
}

template <typename T>
void test_vector_move_assignment_safety()
{
    int n = 100;

    // Step 1: Create and fill original vector
    Vector<T> v1(n);
    for (int i = 0; i < n; i++)
        v1(i) = T(10.5 * i);

    // Step 2: Move-assign into another vector
    Vector<T> v2(1);    // different size to ensure real swap occurs
    v2 = std::move(v1); // calls move assignment

    // Step 3: Validate contents transferred correctly
    for (int i = 0; i < n; i++)
        REQUIRE(v2(i) == T(10.5 * i));

    // Step 4: Validate moved-from vector is in a valid state
    REQUIRE(v1.Size() == 1);

    // Step 5: Ensure that we can safely reuse or destroy moved-from object
    // (should not crash, double-free, or leak)
    v1 = Vector<T>(50); // reassign a new vector after move
    for (int i = 0; i < 50; i++)
        v1(i) = T(i);
    for (int i = 0; i < 50; i++)
        REQUIRE(v1(i) == T(i));

    // Step 6: All destructors run when scope ends — expect no crash or sanitizer errors
    REQUIRE(true);
}

template <typename T>
void test_vector_Size()
{
    int n1 = 0;
    int n2 = 10;
    int n3 = 100;

    Vector<T> v0(n1);
    Vector<T> v1(n2);
    Vector<T> v2(n3);

    REQUIRE(v0.Size() == static_cast<size_t>(n1));
    REQUIRE(v1.Size() == static_cast<size_t>(n2));
    REQUIRE(v2.Size() == static_cast<size_t>(n3));

    // Optional: after copy
    Vector<T> v3(v2);
    REQUIRE(v3.Size() == static_cast<size_t>(n3));

    // Optional: after move
    Vector<T> v4(std::move(v2));
    REQUIRE(v4.Size() == static_cast<size_t>(n3));
    REQUIRE(v2.Size() == 0); // if move sets size to 0
}

template <typename T>
void test_vector_element_access()
{
    int n = 100;
    Vector<T> v(n);

    // Test writing with non-const operator()
    for (size_t i = 0; i < n; ++i)
        v(i) = T(i * 1.5);

    // Test reading with non-const operator()
    for (size_t i = 0; i < n; ++i)
        REQUIRE(v(i) == T(i * 1.5));

    // Test reading with const operator()
    const Vector<T> &cv = v;
    for (size_t i = 0; i < n; ++i)
        REQUIRE(cv(i) == T(i * 1.5));

    // Optional: test modification does not affect copy
    Vector<T> v2 = v; // copy
    v2(0) = T(999);
    REQUIRE(v2(0) == T(999));
    REQUIRE(v(0) == T(0)); // original unchanged
}

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
void test_vector_sub()
{
    int n = 100;

    Vector<T> v1(n);
    Vector<S> v2(n);

    using U = decltype(T{} - S{});

    std::valarray<U> w(n);

    for (int i = 0; i < n; i++)
    {
        v1(i) = T(i);      // cast i to T (works for int, double, complex)
        v2(i) = S(10 * i); // cast 10*i to S

        w[i] = T(i) - S(10 * i); // cast loop variables to match scalar types
    }

    Vector<U> vout = v1 - v2;

    for (int i = 0; i < n; i++)
    {
        REQUIRE(vout(i) == w[i]);
    }
}

template <typename T, typename S>
void test_vector_scalar_mult()
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

template <typename T, typename S>
void test_vector_vector_mult()
{
    int n = 100;

    Vector<T> a(n);
    Vector<S> b(n);

    using U = decltype(T{} * S{});

    U sum = 0;

    for (int i = 0; i < n; i++)
    {
        a(i) = T(i);      // cast i to T (works for int, double, complex)
        b(i) = S(10 * i); // cast 10*i to S

        sum += T(i) * S(10 * i); // cast loop variables to match scalar types
    }

    U out = (a * b)(0);

    REQUIRE(out == sum);
}

TEST_CASE("Vector constructor works for multiple types", "[math]")
{
    SECTION("int") { test_vector_constructor<int>(); }
    SECTION("double") { test_vector_constructor<double>(); }
    SECTION("complex") { test_vector_constructor<complex>(); }
}

TEST_CASE("Vector copy constructor works for multiple types", "[math]")
{
    SECTION("int") { test_vector_copy_constructor<int>(); }
    SECTION("double") { test_vector_copy_constructor<double>(); }
    SECTION("complex") { test_vector_copy_constructor<complex>(); }
}

TEST_CASE("Vector move constructor works for multiple types", "[math]")
{
    SECTION("int") { test_vector_move_constructor<int>(); }
    SECTION("double") { test_vector_move_constructor<double>(); }
    SECTION("complex") { test_vector_move_constructor<complex>(); }
}

TEST_CASE("Vector copy and destructor interact safely")
{
    test_vector_destructor<int>();
    test_vector_destructor<double>();
    test_vector_destructor<complex>();
}

TEST_CASE("Vector copy assignment operator works for multiple types", "[math]")
{
    SECTION("int") { test_vector_copy_assignment<int>(); }
    SECTION("double") { test_vector_copy_assignment<double>(); }
    SECTION("complex") { test_vector_copy_assignment<complex>(); }
}

TEST_CASE("Vector move assignment operator is memory-safe", "[math]")
{
    SECTION("int") { test_vector_move_assignment_safety<int>(); }
    SECTION("double") { test_vector_move_assignment_safety<double>(); }
    SECTION("complex") { test_vector_move_assignment_safety<complex>(); }
}

TEST_CASE("Vector Size() returns correct value", "[math]")
{
    SECTION("int") { test_vector_Size<int>(); }
    SECTION("double") { test_vector_Size<double>(); }
    SECTION("complex") { test_vector_Size<complex>(); }
}

TEST_CASE("Vector element access operators work correctly", "[math]")
{
    SECTION("int") { test_vector_element_access<int>(); }
    SECTION("double") { test_vector_element_access<double>(); }
    SECTION("complex") { test_vector_element_access<complex>(); }
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

TEST_CASE("Vector subtraction works for multiple types", "[math]")
{
    SECTION("4int") { test_vector_sub<int, int>(); }
    SECTION("4double") { test_vector_sub<double, double>(); }
    SECTION("4complex") { test_vector_sub<complex, complex>(); }
    SECTION("2double2int") { test_vector_sub<double, int>(); }
    SECTION("2complex2double") { test_vector_sub<complex, double>(); }
    SECTION("2double2complex") { test_vector_sub<double, complex>(); }
}

TEST_CASE("Vector scalar-multiplication works for multiple types", "[math]")
{
    SECTION("4int") { test_vector_scalar_mult<int, int>(); }
    SECTION("4double") { test_vector_scalar_mult<double, double>(); }
    SECTION("4complex") { test_vector_scalar_mult<complex, complex>(); }
    SECTION("2double2int") { test_vector_scalar_mult<double, int>(); }
}

TEST_CASE("Vector vector-multiplication works for multiple types", "[math]")
{
    SECTION("4int") { test_vector_vector_mult<int, int>(); }
    SECTION("4double") { test_vector_vector_mult<double, double>(); }
    SECTION("4complex") { test_vector_vector_mult<complex, complex>(); }
    SECTION("2double2int") { test_vector_vector_mult<double, int>(); }
}
