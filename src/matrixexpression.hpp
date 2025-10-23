/*
Author: Edoardo Bonetti
*/

#ifndef FILE_MAT_EXPRESSION
#define FILE_MAT_EXPRESSION

#include "vector.hpp"
#include "simd.h"

namespace bla_ga
{
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

      U sum0 = U(0);
      U sum1 = U(0);
      U sum2 = U(0);
      U sum3 = U(0);

      size_t n = a.nCols();
      size_t unrolled_end = n - n % 4; // last index for unrolled loop

      // Main unrolled loop
      for (size_t k = 0; k < unrolled_end; k += 4)
      {
        // make TileMatExpr
        sum0 += a(i, k) * b(k, j);
        sum1 += a(i, k + 1) * b(k + 1, j);
        sum2 += a(i, k + 2) * b(k + 2, j);
        sum3 += a(i, k + 3) * b(k + 3, j);
      }

      // Remainder loop for leftover elements
      for (size_t k = unrolled_end; k < n; k++)
      {
        sum0 += a(i, k) * b(k, j);
      }

      U sum = sum0 + sum1 + sum2 + sum3;
      return sum;
    }

    size_t nRows() const { return a.nRows(); }
    size_t nCols() const { return b.nCols(); }

    //    template <typename MatType>
    //    void Eval(MatType &C) const
    //    {
    //      size_t M = a.nRows();
    //      size_t N = b.nCols();
    //      size_t K = a.nCols();
    //
    //      // Initialize C to zero
    //      for (size_t i = 0; i < M; ++i)
    //        for (size_t j = 0; j < N; ++j)
    //          C(i, j) = 0;
    //
    //      // Loop reordering for better cache performance
    //      for (size_t i = 0; i < M; ++i)
    //        for (size_t k = 0; k < K; ++k)
    //        {
    //          auto aik = a(i, k);
    //          for (size_t j = 0; j < N; ++j)
    //            C(i, j) += aik * b(k, j);
    //        }
    //    }

    template <typename MatType>
    void Eval(MatType &C) const
    {
      const size_t BS = 64; // block size
      size_t M = a.nRows(), N = b.nCols(), K = a.nCols();

      for (size_t ii = 0; ii < M; ii += BS)
        for (size_t kk = 0; kk < K; kk += BS)
          for (size_t jj = 0; jj < N; jj += BS)
            for (size_t i = ii; i < std::min(ii + BS, M); ++i)
              for (size_t k = kk; k < std::min(kk + BS, K); ++k)
              {
                auto aik = a(i, k);
                for (size_t j = jj; j < std::min(jj + BS, N); ++j)
                  C(i, j) += aik * b(k, j);
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
