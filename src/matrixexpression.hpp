/*
Author: Edoardo Bonetti
*/

#ifndef FILE_MAT_EXPRESSION
#define FILE_MAT_EXPRESSION

#include "vector.hpp"
#include "simd.h"
#include <algorithm>

#ifdef _OPENMP
#include <omp.h>
#endif
namespace bla_ga
{

  // create a prename for MatrixView
  // Bring them into local scope

  /*------------MatExpr------------*/
  template <typename T>
  class MatExpr
  {
  public:
    auto Upcast() const { return static_cast<const T &>(*this); }
    size_t nCols() const { return Upcast().nCols(); }
    size_t nRows() const { return Upcast().nRows(); }
    auto operator()(size_t i, size_t j) const { return Upcast()(i, j); }
  };

  /*------------SumMatExpr------------*/
  template <typename TA, typename TB>
  class SumMatExpr : public MatExpr<SumMatExpr<TA, TB>>
  {
    TA a;
    TB b;

  public:
    SumMatExpr(TA _a, TB _b) : a(_a), b(_b) {}

    auto operator()(size_t i, size_t j) const { return a(i, j) + b(i, j); }
    size_t nRows() const { return a.nRows(); }
    size_t nCols() const { return a.nCols(); }
  };

  template <typename TA, typename TB>
  auto operator+(const MatExpr<TA> &a, const MatExpr<TB> &b)
  {
    return SumMatExpr(a.Upcast(), b.Upcast());
  }

  /*------------ScaleMatExpr------------*/
  template <typename TSCAL, typename T>
  class ScaleMatExpr : public MatExpr<ScaleMatExpr<TSCAL, T>>
  {
    TSCAL scal;
    T mat;

  public:
    ScaleMatExpr(TSCAL _scal, T _mat) : scal(_scal), mat(_mat) {}

    auto operator()(size_t i, size_t j) const { return scal * mat(i, j); }
    size_t nRows() const { return mat.nRows(); }
    size_t nCols() const { return mat.nCols(); }
  };

  template <typename T>
  auto operator*(double scal, const MatExpr<T> &m)
  {
    return ScaleMatExpr(scal, m.Upcast());
  }

  /*------------MultMatExpr------------*/
  template <typename TA, typename TB>
  class MultMatExpr : public MatExpr<MultMatExpr<TA, TB>>
  {
    TA a;
    TB b;

  public:
    MultMatExpr(TA _a, TB _b) : a(_a), b(_b) {}

    auto operator()(size_t i, size_t j) const
    {
      using U = decltype(a(0, 0) * b(0, 0));
      size_t n = a.nCols();
      U sum0 = 0;
      U sum1 = 0;
      size_t k = 0;
      for (; k + 2 <= n; k += 2)
      {
        sum0 += a(i, k) * b(k, j);
        sum1 += a(i, k + 1) * b(k + 1, j);
      }
      // take care of the leftover
      if (k < n)
        sum0 += a(i, k) * b(k, j);

      return sum0 + sum1;
    }

    size_t
    nRows() const
    {
      return a.nRows();
    }
    size_t nCols() const { return b.nCols(); }

    // Best so far:

    template <typename MatType> // does inline help?
    inline void Eval(MatType &C) const
    {
      std::cout << "Using simple Eval" << std::endl;

      // PRINT THE TYPE OF decltype(TA{} * TB{})
      std::cout << "Type of matrix multiplication elements: " << typeid(decltype(a(0, 0) * b(0, 0))).name() << std::endl;

      size_t M = a.nRows(), N = b.nCols(), K = a.nCols();

      constexpr size_t BS = 256;
      constexpr size_t RS = 4;

      for (size_t ii = 0; ii < M; ii += BS)
        for (size_t kk = 0; kk < K; kk += BS)
          for (size_t jj = 0; jj < N; jj += BS)
            for (size_t i = ii; i < std::min(ii + BS, M); i += RS)
              for (size_t j = jj; j < std::min(jj + BS, N); j += RS)
              {
                double cloc[RS][RS] = {0.0};

                size_t tileM = std::min(RS, M - i);
                size_t tileN = std::min(RS, N - j);

                // accumulate into cloc
                for (size_t k = kk; k < std::min(kk + BS, K); ++k)
                  for (size_t ii2 = 0; ii2 < tileM; ++ii2)
                    for (size_t jj2 = 0; jj2 < tileN; ++jj2)
                      cloc[ii2][jj2] += a(i + ii2, k) * b(k, j + jj2);

                // store back to C once
                for (size_t ii2 = 0; ii2 < tileM; ++ii2)
                  for (size_t jj2 = 0; jj2 < tileN; ++jj2)
                    C(i + ii2, j + jj2) += cloc[ii2][jj2];
              }
    }

    template <typename MatType>
      requires std::is_same_v<decltype(a(0, 0) * b(0, 0)), double>
    inline void EvalSIMD(MatType &C) const
    {
      std::cout << "Using simple SIMD Eval" << std::endl;

      // PRINT THE TYPE OF decltype(TA{} * TB{})
      std::cout << "Type of matrix multiplication elements: " << typeid(decltype(a(0, 0) * b(0, 0))).name() << std::endl;

      size_t M = a.nRows(), N = b.nCols(), K = a.nCols();

      constexpr size_t BS = 128;
      constexpr size_t RS = 4;

      for (size_t ii = 0; ii < M; ii += BS)
        for (size_t kk = 0; kk < K; kk += BS)
          for (size_t jj = 0; jj < N; jj += BS)
            for (size_t i = ii; i < std::min(ii + BS, M); i += RS)
              for (size_t j = jj; j < std::min(jj + BS, N); j += RS)
              {
                double cloc[RS][RS] = {0.0};

                size_t tileM = std::min(RS, M - i);
                size_t tileN = std::min(RS, N - j);

                // accumulate into cloc
                for (size_t k = kk; k < std::min(kk + BS, K); ++k)
                  for (size_t ii2 = 0; ii2 < tileM; ++ii2)
                    for (size_t jj2 = 0; jj2 < tileN; ++jj2)
                      cloc[ii2][jj2] += a(i + ii2, k) * b(k, j + jj2);

                // store back to C once
                for (size_t ii2 = 0; ii2 < tileM; ++ii2)
                  for (size_t jj2 = 0; jj2 < tileN; ++jj2)
                    C(i + ii2, j + jj2) += cloc[ii2][jj2];
              }
    }
  };

  template <typename TA, typename TB>
  auto operator*(const MatExpr<TA> &a, const MatExpr<TB> &b)
  {
    return MultMatExpr(a.Upcast(), b.Upcast());
  }

  /*------------operator<<------------*/
  template <typename T>
  std::ostream &operator<<(std::ostream &ost, const MatExpr<T> &m)
  {
    if (m.nCols() > 0 && m.nRows() > 0)
      for (size_t i = 0; i < m.nRows(); i++)
      {
        for (size_t j = 0; j < m.nCols(); j++)
        {
          ost << " " << m(i, j);
        }
        ost << std::endl;
      }

    return ost;
  }

} // namespace bla_ga
#endif
