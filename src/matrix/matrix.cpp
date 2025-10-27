#include <iostream>
#include <chrono>
#include <immintrin.h>
#include <functional>

/*

BEST OPTION IS THE ONE WITH SIMD at the end

*/


template<typename T, size_t S>
struct SIMD {
    T data[S];

    SIMD() { for (size_t i = 0; i < S; ++i) data[i] = 0; }

    SIMD& operator+=(const SIMD& other) {
        for (size_t i = 0; i < S; ++i) data[i] += other.data[i];
        return *this;
    }

    SIMD operator*(const SIMD& other) const {
        SIMD result;
        for (size_t i = 0; i < S; ++i) result.data[i] = data[i] * other.data[i];
        return result;
    }


    T horizontal_sum() const {
        T sum = 0;
        for (size_t i = 0; i < S; ++i) sum += data[i];
        return sum;
    }
};



// Updated SIMD<double,4>
template<>
struct SIMD<double,4> {
    __m256d data;

    SIMD() : data(_mm256_setzero_pd()) {}
    SIMD(const __m256d& d) : data(d) {}
    SIMD(double val) : data(_mm256_set1_pd(val)) {}

    SIMD& operator+=(const SIMD& other) {
        data = _mm256_add_pd(data, other.data);
        return *this;
    }

    SIMD operator*(const SIMD& other) const {
        return SIMD(_mm256_mul_pd(data, other.data));
    }

    // FMA with another SIMD
    SIMD& fmadd(const SIMD& a, const SIMD& b) {
        data = _mm256_fmadd_pd(a.data, b.data, data);
        return *this;
    }

    // FMA with scalar
    SIMD& fmadd_scalar(const SIMD& a, double b) {
        data = _mm256_fmadd_pd(a.data, _mm256_set1_pd(b), data);
        return *this;
    }

    double horizontal_sum() const {
        alignas(32) double temp[4];
        _mm256_store_pd(temp, data);
        return temp[0] + temp[1] + temp[2] + temp[3];
    }

    static SIMD load(const double* ptr) {
        return SIMD(_mm256_loadu_pd(ptr));
    }

    static SIMD set(double a0, double a1, double a2, double a3) {
        return SIMD(_mm256_setr_pd(a0, a1, a2, a3));
    }

    static SIMD<double,4> masked_load(const double* ptr, size_t count) {
        alignas(32) double temp[4] = {0.0, 0.0, 0.0, 0.0};
        for (size_t i = 0; i < count; ++i) temp[i] = ptr[i];
        return SIMD(_mm256_load_pd(temp));
    }
};

class Matrix {
private:
    size_t rows_, cols_;
    double* data_;

public:
    Matrix(size_t rows, size_t cols)
        : rows_(rows), cols_(cols), data_(new double[rows * cols]()) {}

    // Copy constructor
    Matrix(const Matrix& other)
        : rows_(other.rows_), cols_(other.cols_), data_(new double[rows_ * cols_]) {
        std::copy(other.data_, other.data_ + rows_ * cols_, data_);
    }

    // Move constructor
    Matrix(Matrix&& other) noexcept
        : rows_(other.rows_), cols_(other.cols_), data_(other.data_) {
        other.data_ = nullptr;
        other.rows_ = other.cols_ = 0;
    }

    // Copy assignment
    Matrix& operator=(const Matrix& other) {
        if (this != &other) {
            if (rows_ != other.rows_ || cols_ != other.cols_) {
                delete[] data_;
                rows_ = other.rows_;
                cols_ = other.cols_;
                data_ = new double[rows_ * cols_];
            }
            std::copy(other.data_, other.data_ + rows_ * cols_, data_);
        }
        return *this;
    }

    // Move assignment
    Matrix& operator=(Matrix&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            rows_ = other.rows_;
            cols_ = other.cols_;
            data_ = other.data_;
            other.data_ = nullptr;
            other.rows_ = other.cols_ = 0;
        }
        return *this;
    }

    ~Matrix() {
        delete[] data_;
    }

    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }

    double& operator()(size_t i, size_t j) {
        return data_[i * cols_ + j];
    }

    const double& operator()(size_t i, size_t j) const {
        return data_[i * cols_ + j];
    }

    void print() const {
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t j = 0; j < cols_; ++j) {
                std::cout << (*this)(i, j) << " ";
            }
            std::cout << '\n';
        }
    }
};

// micro-kernel: multiplies A( M x K ) * B( K x N ) and accumulates into C( M x N ).
// A, B, C are pointers to the starting element of the submatrices.
// lda, ldb, ldc are the full matrix leading dimensions (number of columns).
// M, N, K are tile sizes (may be <= 4 for tails).


void pack_B_tile_2(const double* B, size_t ldb, size_t K_block, size_t N_block, double* packedB)
{
    // Column-major packing for better reuse in kernel
    // B: (K_block × N_block)
    for (size_t j = 0; j < N_block; ++j) {
        const double* col = B + j;
        for (size_t k = 0; k < K_block; ++k) {
            packedB[j * K_block + k] = col[k * ldb];
        }
    }
}
static inline void micro_kernel_4x4_packedB_unrollK_opt(const double* A, const double* packedB, double* C,
                                                       size_t lda, size_t ldc,
                                                       size_t M, size_t N, size_t K)
{
    constexpr size_t UNROLL = 4; // K-loop unroll factor
    __m256d c0 = _mm256_setzero_pd();
    __m256d c1 = _mm256_setzero_pd();
    __m256d c2 = _mm256_setzero_pd();
    __m256d c3 = _mm256_setzero_pd();

    size_t k = 0;

    auto loadA = [&](size_t k_idx) -> __m256d {
        // Load up to 4 rows of column k_idx from A directly into register
        double a0 = (M > 0) ? A[0*lda + k_idx] : 0.0;
        double a1 = (M > 1) ? A[1*lda + k_idx] : 0.0;
        double a2 = (M > 2) ? A[2*lda + k_idx] : 0.0;
        double a3 = (M > 3) ? A[3*lda + k_idx] : 0.0;
        return _mm256_setr_pd(a0, a1, a2, a3);
    };

    // Main unrolled loop
    for (; k + UNROLL <= K; k += UNROLL) {
        for (size_t u = 0; u < UNROLL; ++u) {
            __m256d a_vec = loadA(k + u);

            if (N >= 1) c0 = _mm256_fmadd_pd(a_vec, _mm256_set1_pd(packedB[0*K + k + u]), c0);
            if (N >= 2) c1 = _mm256_fmadd_pd(a_vec, _mm256_set1_pd(packedB[1*K + k + u]), c1);
            if (N >= 3) c2 = _mm256_fmadd_pd(a_vec, _mm256_set1_pd(packedB[2*K + k + u]), c2);
            if (N >= 4) c3 = _mm256_fmadd_pd(a_vec, _mm256_set1_pd(packedB[3*K + k + u]), c3);
        }
    }

    // Tail loop
    for (; k < K; ++k) {
        __m256d a_vec = loadA(k);

        if (N >= 1) c0 = _mm256_fmadd_pd(a_vec, _mm256_set1_pd(packedB[0*K + k]), c0);
        if (N >= 2) c1 = _mm256_fmadd_pd(a_vec, _mm256_set1_pd(packedB[1*K + k]), c1);
        if (N >= 3) c2 = _mm256_fmadd_pd(a_vec, _mm256_set1_pd(packedB[2*K + k]), c2);
        if (N >= 4) c3 = _mm256_fmadd_pd(a_vec, _mm256_set1_pd(packedB[3*K + k]), c3);
    }

    // Store results back to C
    double tmp[4];
    if (N >= 1) { _mm256_storeu_pd(tmp, c0); for (size_t i=0;i<M;i++) C[i*ldc +0] += tmp[i]; }
    if (N >= 2) { _mm256_storeu_pd(tmp, c1); for (size_t i=0;i<M;i++) C[i*ldc +1] += tmp[i]; }
    if (N >= 3) { _mm256_storeu_pd(tmp, c2); for (size_t i=0;i<M;i++) C[i*ldc +2] += tmp[i]; }
    if (N >= 4) { _mm256_storeu_pd(tmp, c3); for (size_t i=0;i<M;i++) C[i*ldc +3] += tmp[i]; }
}



// Optimized 4x4 micro-kernel using SIMD<double,4>
static inline void micro_kernel_4x4_packedB_SIMD(
    const double* __restrict__ A,
    const double* __restrict__ packedB,
    double* __restrict__ C,
    size_t lda, size_t ldc,
    size_t M, size_t N, size_t K)
{
    SIMD<double,4> c0, c1, c2, c3;

    for (size_t k = 0; k < K; ++k) {
        // Load one column of A (up to 4 rows)
        double a0 = (M > 0) ? A[0*lda + k] : 0.0;
        double a1 = (M > 1) ? A[1*lda + k] : 0.0;
        double a2 = (M > 2) ? A[2*lda + k] : 0.0;
        double a3 = (M > 3) ? A[3*lda + k] : 0.0;
        SIMD<double,4> a_vec = SIMD<double,4>::set(a0,a1,a2,a3);

        // FMA with packedB
        if (N >= 1) c0.fmadd_scalar(a_vec, packedB[0*K + k]);
        if (N >= 2) c1.fmadd_scalar(a_vec, packedB[1*K + k]);
        if (N >= 3) c2.fmadd_scalar(a_vec, packedB[2*K + k]);
        if (N >= 4) c3.fmadd_scalar(a_vec, packedB[3*K + k]);
    }

    // Store results back to C (handling tails safely)
    alignas(32) double tmp[4];

    auto store = [&](SIMD<double,4>& v, size_t col) {
        _mm256_store_pd(tmp, v.data);
        for (size_t i = 0; i < M; ++i)
            C[i*ldc + col] += tmp[i];
    };

    if (N >= 1) store(c0, 0);
    if (N >= 2) store(c1, 1);
    if (N >= 3) store(c2, 2);
    if (N >= 4) store(c3, 3);
}


void multiply_tiles_packedB_SIMD(
    const Matrix& A, const Matrix& B, Matrix& C,
    size_t i0, size_t j0, size_t k0,
    size_t M_block, size_t N_block, size_t K_block)
{
    const size_t lda = A.cols();
    const size_t ldc = C.cols();

    // 32-byte aligned packed buffer for B
    double* packedB = static_cast<double*>(_mm_malloc(K_block * N_block * sizeof(double), 32));
    pack_B_tile_2(&B(k0, j0), B.cols(), K_block, N_block, packedB);

    for (size_t ii = 0; ii < M_block; ii += 4) {
        size_t Mtail = std::min((size_t)4, M_block - ii);
        for (size_t jj = 0; jj < N_block; jj += 4) {
            size_t Ntail = std::min((size_t)4, N_block - jj);

            double* cptr = &C(i0 + ii, j0 + jj);
            const double* aptr = &A(i0 + ii, k0);
            const double* bptr = packedB + (jj * K_block);

            micro_kernel_4x4_packedB_SIMD(aptr, bptr, cptr, lda, ldc, Mtail, Ntail, K_block);
        }
    }

    _mm_free(packedB);
}

void multiply(const Matrix& A, const Matrix& B, Matrix& C)
{
    size_t M = A.rows();
    size_t K = A.cols();
    size_t K2 = B.rows();
    size_t N = B.cols();
    if (K != K2) {
        std::cerr << "mult_rect: inner dims mismatch: A.cols()=" << K
                  << " B.rows()=" << K2 << std::endl;
        std::exit(1);
    }

    //const size_t BS = 6*64; // block size (tuneable)
    const size_t BM = 128; // block size for rows of A / C
    const size_t BN = 128; // block size for cols of B / C
    const size_t BK = 64;  // block size for inner K dimension
    
    //for (size_t ii = 0; ii < M; ii += BS) {
    //    for (size_t jj = 0; jj < N; jj += BS) {
    //        for (size_t kk = 0; kk < K; kk += BS) {
    //            size_t M_block = std::min(BS, M - ii);
    //            size_t N_block = std::min(BS, N - jj);
    //            size_t K_block = std::min(BS, K - kk);
    //            multiply_tiles_packedB_unrollK(A, B, C, ii, jj, kk, M_block, N_block, K_block);                
    //        }
    //    }
    //}

    for (size_t ii = 0; ii < M; ii += BM)
    for (size_t kk = 0; kk < K; kk += BK)    
    for (size_t jj = 0; jj < N; jj += BN)
            
            {
                size_t M_block = std::min(BM, M - ii);
                size_t N_block = std::min(BN, N - jj);
                size_t K_block = std::min(BK, K - kk);

                //multiply_tiles_packedB_unrollK_2(A, B, C,
                //                               ii, jj, kk,
                //                               M_block, N_block, K_block);
                multiply_tiles_packedB_SIMD(A, B, C, ii, jj, kk, M_block, N_block, K_block);

            }
}


inline void multiply_base(const Matrix& a, const Matrix& b, Matrix& c) {
    size_t m= a.rows();
    size_t n = b.cols();
    size_t p = a.cols();


    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < p; ++k) {
                sum += a(i,k) * b(k,j);
            }
            c(i,j) = sum;
        }
    }
}


template <typename Func>
double chronoFunction(Func f, const Matrix& a, const Matrix& b, Matrix& c, int niter = 1) {
    using clock = std::chrono::high_resolution_clock;
    auto start = clock::now();

    for (int iter = 0; iter < niter; ++iter) {
        f(a, b, c);
    }

    auto end = clock::now();
    std::chrono::duration<double> duration = end - start;

    // FLOPs for matrix multiplication: 2 * M * N * K per iteration
    double M = (double)a.rows();
    double K = (double)a.cols();
    double K2 = (double)b.rows();
    double N = (double)b.cols();
    if (K != K2) {
        std::cerr << "chronoFunction: inner dims mismatch\n";
        return 0.0;
    }
    double flops_per_iter = 2.0 * M * N * K;
    double gflops = (flops_per_iter * niter) / (duration.count() * 1e9);

    std::cout << "Time: " << duration.count() / niter << " s per iteration, "
              << "GFLOPS: " << gflops << std::endl;

    return gflops;
}



