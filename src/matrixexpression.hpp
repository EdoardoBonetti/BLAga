/*
Author: Edoardo Bonetti
*/

#ifndef FILE_MAT_EXPRESSION
#define FILE_MAT_EXPRESSION

#include "matrix_fwd.hpp" // ✅ instead of redeclaring enum or templates
#include "vector.hpp"
#include "matrixexpressionspecialization.hpp"

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

    size_t nRows() const
    {
      return a.nRows();
    }
    size_t nCols() const { return b.nCols(); }

    // Best so far:

    template <typename MatType> // does inline help?
    inline void Eval(MatType &C) const
    {
      // Delegate to specialized implementation
      const auto &a = this->a;
      const auto &b = this->b;
      EvalMatMatMultiplyDouble(a, b, C);
    }

    template <typename MatType>
      requires std::is_same_v<decltype(a(0, 0) * b(0, 0)), double>
    inline void EvalSIMD(MatType &C) const
    {
      // Delegate to specialized implementation
      const auto &a = this->a;
      const auto &b = this->b;
      SIMDEvalMatMatMultiplyDouble(a, b, C);
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
