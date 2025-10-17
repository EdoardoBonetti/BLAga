#ifndef FILE_MAT_EXPRESSION
#define FILE_MAT_EXPRESSION

#include "vector.hpp"

namespace bla_ga
{

  template <typename T>
  class MatExpr
  {
  public:
    auto Upcast() const { return static_cast<const T &>(*this); }
    size_t nCols() const { return Upcast().nCols(); }
    size_t nRows() const { return Upcast().nRows(); }
    auto operator()(size_t i, size_t j) const { return Upcast()(i, j); }
  };

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

  template <typename TA, typename TB>
  class MultMatExpr : public MatExpr<MultMatExpr<TA, TB>>
  {
    TA a;
    TB b;

  public:
    MultMatExpr(TA _a, TB _b) : a(_a), b(_b) {}

    auto operator()(size_t i, size_t j) const
    {
      // using A = std::invoke_result<TA, size_t>::type;
      // using B = std::invoke_result<TB, size_t>::type;
      //  using U = decltype(std::declval<A>() * std::declval<B>());

      using U = decltype(a(0, 0) * b(0, 0));

      U sum = U(0);

      for (size_t k = 0; k < a.nCols(); k++)
        sum += a(i, k) * b(k, j);
      return sum;
    }

    size_t nRows() const { return a.nRows(); }
    size_t nCols() const { return b.nCols(); }
  };

  template <typename TA, typename TB>
  auto operator*(const MatExpr<TA> &a, const MatExpr<TB> &b)
  {
    return MultMatExpr(a.Upcast(), b.Upcast());
  }

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

}

#endif
