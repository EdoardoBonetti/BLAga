#include <iostream>
#include <chrono>
#include <functional>
#include <arm_neon.h>
#include "matrix.hpp"
// for writing
#include <ostream>
// take the time
using clock_type = std::chrono::high_resolution_clock;
/*

BEST OPTION IS THE ONE WITH SIMD at the end

*/

template <typename T, size_t S>
struct SIMD
{
    T data[S];

    SIMD()
    {
        for (size_t i = 0; i < S; ++i)
            data[i] = 0;
    }

    SIMD &operator+=(const SIMD &other)
    {
        for (size_t i = 0; i < S; ++i)
            data[i] += other.data[i];
        return *this;
    }

    SIMD operator*(const SIMD &other) const
    {
        SIMD result;
        for (size_t i = 0; i < S; ++i)
            result.data[i] = data[i] * other.data[i];
        return result;
    }

    T horizontal_sum() const
    {
        T sum = 0;
        for (size_t i = 0; i < S; ++i)
            sum += data[i];
        return sum;
    }
};

// Updated SIMD<double,4>
template <>
struct SIMD<double, 2>
{
    float64x2_t data;

    // Default constructor initializes to zero
    SIMD()
    {
        data = vdupq_n_f64(0.0);
    }

    // Constructor to initialize all elements to a specific value
    SIMD(double val)
    {
        data = vdupq_n_f64(val);
    }

    // Constructor to initialize two elements
    SIMD(double val1, double val2)
    {
        alignas(16) double tmp[2] = {val1, val2};
        data = vld1q_f64(tmp);
    }

    // Cnstrucor from array
    SIMD(const double vals[2])
    {
        alignas(16) double tmp[2] = {vals[0], vals[1]};
        data = vld1q_f64(tmp);
    }

    // Constructor from pointer
    SIMD(double *ptr)
    {
        data = vld1q_f64(ptr);
    }

    // constructor for masked load
    // crete a simd that is 0,0 or 1,0 depending id count is even or odd
    SIMD(const double *ptr, size_t remaining)
    {
        alignas(16) double temp[2] = {0.0, 0.0};
        for (size_t i = 0; i < remaining; ++i)
            temp[i] = ptr[i];
        data = vld1q_f64(temp);
    }

    // Copy constructor
    SIMD(const SIMD &other)
    {
        data = other.data;
    }

    // Move constructor
    SIMD(SIMD &&other) noexcept
    {
        data = other.data;
    }
    SIMD &operator=(const SIMD &other)
    {
        data = other.data;
        return *this;
    }

    // Addition operator
    SIMD &operator+=(const SIMD &other)
    {
        data = vaddq_f64(data, other.data);
        return *this;
    }

    SIMD operator+(const SIMD &other) const
    { // Addition operator: element-wise addition
        SIMD result;
        result.data = vaddq_f64(data, other.data);
        return result;
    }

    // ToTal sum between two SIMD
    double total_sum(const SIMD &other) const
    {
        SIMD sum_vec;
        sum_vec.data = vaddq_f64(data, other.data);
        double sum_array[2];
        vst1q_f64(sum_array, sum_vec.data);
        return sum_array[0] + sum_array[1];
    }

    // Subtraction operator
    SIMD &operator-=(const SIMD &other)
    {
        data = vsubq_f64(data, other.data);
        return *this;
    }

    // Multiplication operator
    SIMD operator*(const SIMD &other) const
    { // Multiplication operator: element-wise multiplication
        SIMD result;
        result.data = vmulq_f64(data, other.data);
        return result;
    }

    double scalar_product(const SIMD &other) const
    { // Dot product
        float64x2_t mul = vmulq_f64(data, other.data);
        double sum_array[2];
        vst1q_f64(sum_array, mul);
        return sum_array[0] + sum_array[1];
    }

    // multiply by scalar
    SIMD operator*(double scalar) const
    {
        SIMD result;
        float64x2_t scalar_vec = vdupq_n_f64(scalar);
        result.data = vmulq_f64(data, scalar_vec);
        return result;
    }

    double horizontal_sum() const
    {
        double sum_array[2];
        vst1q_f64(sum_array, data);
        return sum_array[0] + sum_array[1];
    };

    void Store(double *ptr) const
    {
        vst1q_f64(ptr, data);
    }

    friend std::ostream &operator<<(std::ostream &os, const SIMD &simd)
    {
        alignas(16) double vals[2];
        vst1q_f64(vals, simd.data);
        os << "[" << vals[0] << ", " << vals[1] << "]";
        return os;
    }
    static SIMD mul_add(const SIMD &a, const SIMD &b, const SIMD &acc)
    {
        SIMD result;
        result.data = vmlaq_f64(acc.data, a.data, b.data);
        return result;
    }

    inline static void mul_add_inplace(SIMD &acc, const SIMD &a, const SIMD &b)
    {
        acc.data = vmlaq_f64(acc.data, a.data, b.data); // acc += a * b
    }
};

// ----------------------------------------------------------------------
namespace bla_ga
{

    template <typename TA, typename TB, typename TC>
    void multiply_base(const TA a, const TB b, TC c)
    {
        for (size_t i = 0; i < a.nRows(); i++)
            for (size_t j = 0; j < b.nCols(); j++)
            {
                double sum = 0.0;
                for (size_t k = 0; k < a.nCols(); k++)
                    sum += a(i, k) * b(k, j);
                c(i, j) = sum;
            }
    }

    void multiply_tiles(const Matrix<> a, const Matrix<> b, Matrix<> c, size_t ii,
                        size_t jj, size_t kk, size_t M_block, size_t N_block,
                        size_t K_block)
    {
    }

    void multiplyADV(const Matrix<> &a, const Matrix<> b, Matrix<> c)
    {
        size_t M = a.nRows();
        size_t P = a.nCols();
        size_t N = b.nCols();

        size_t BM = 20; // rows of A
        size_t BK = 20; // inner dimension
        size_t BN = 20; // cols of B

        for (size_t ii = 0; ii < M; ii += BM)
        {
            for (size_t jj = 0; jj < N; jj += BN)
            {
                for (size_t kk = 0; kk < P; kk += BK)
                {
                    size_t M_block = std::min(BM, M - ii);
                    size_t N_block = std::min(BN, N - jj);
                    size_t K_block = std::min(BK, P - kk);
                }
            }
        }
    }

    int main()
    {

        size_t M = 1000;
        size_t P = 1000;
        size_t N = 1000;

        Matrix<> A(M, P);
        Matrix<> B(P, N);
        Matrix<> Cref(M, N);
        Matrix<> C(M, N);

        auto start1 = std::chrono::high_resolution_clock::now();
        multiply_base(A, B, Cref);
        auto end1 = std::chrono::high_resolution_clock::now();

        auto start2 = std::chrono::high_resolution_clock::now();
        // multiplyADV(A, B, C);
        auto end2 = std::chrono::high_resolution_clock::now();

        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count() << std::endl;
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count() << std::endl;

        return 0;
    }
}