#ifndef FILE_VEC_EXPRESSION
#define FILE_VEC_EXPRESSION

#include <complex>
#include <cassert>
#include <type_traits>
#include <concepts>

namespace bla_ga
{

  /*------------VecExpr------------*/
  template <typename T>
  class VecExpr
  {
  public:
    auto Upcast() const { return static_cast<const T &>(*this); }
    size_t Size() const { return Upcast().Size(); }
    auto operator()(size_t i) const { return Upcast()(i); }
  };

  /*------------SumVecExpr------------*/

  template <typename TA, typename TB>
  class SumVecExpr : public VecExpr<SumVecExpr<TA, TB>>
  {
    TA a;
    TB b;

  public:
    SumVecExpr(TA _a, TB _b) : a(_a), b(_b) {}

    auto operator()(size_t i) const { return a(i) + b(i); }
    size_t Size() const { return a.Size(); }
  };

  template <typename TA, typename TB>
  auto operator+(const VecExpr<TA> &a, const VecExpr<TB> &b)
  {
    return SumVecExpr(a.Upcast(), b.Upcast());
  }

  /*------------SubVecExpr------------*/
  template <typename TA, typename TB>
  class SubVecExpr : public VecExpr<SubVecExpr<TA, TB>>
  {
    TA a;
    TB b;

  public:
    SubVecExpr(TA _a, TB _b) : a(_a), b(_b) {}

    auto operator()(size_t i) const { return a(i) - b(i); }
    size_t Size() const { return a.Size(); }
  };

  template <typename TA, typename TB>
  auto operator-(const VecExpr<TA> &a, const VecExpr<TB> &b)
  {
    return SubVecExpr(a.Upcast(), b.Upcast());
  }

  /*------------NegVecExpr------------*/
  template <typename TA>
  class NegVecExpr : public VecExpr<NegVecExpr<TA>>
  {
    TA a;

  public:
    NegVecExpr(TA _a) : a(_a) {}

    auto operator()(size_t i) const { return -a(i); }
    size_t Size() const { return a.Size(); }
  };

  template <typename TA>
  auto operator-(const VecExpr<TA> &a)
  {
    return NegVecExpr(-a.Upcast());
  }

  /*------------ScaleVecExpr------------*/
  template <typename T>
  struct is_scalar_type
  {
    static constexpr bool value = std::integral<T> || std::floating_point<T>;
  };

  template <typename T>
  constexpr bool isScalar() { return is_scalar_type<T>::value; }

  template <typename T>
  struct is_scalar_type<std::complex<T>>
  {
    static constexpr bool value = isScalar<T>();
  };

  template <typename TSCAL, typename TV>
  class ScaleVecExpr : public VecExpr<ScaleVecExpr<TSCAL, TV>>
  {
    TSCAL scal;
    TV vec;

  public:
    ScaleVecExpr(TSCAL _scal, TV _vec) : scal(_scal), vec(_vec) {}

    auto operator()(size_t i) const { return scal * vec(i); }
    size_t Size() const { return vec.Size(); }
  };

  template <typename TSCAL, typename T>
    requires(isScalar<TSCAL>())
  auto operator*(TSCAL scal, const VecExpr<T> &v)
  {
    return ScaleVecExpr(scal, v.Upcast());
  }

  /*------------MultVecExpr------------*/
  template <typename TA, typename TB>
  class MultVecExpr : public VecExpr<MultVecExpr<TA, TB>>
  {
    TA a;
    TB b;

  public:
    MultVecExpr(TA _a, TB _b) : a(_a), b(_b) {}

    auto operator()(size_t i) const
    {
      using A = std::invoke_result<TA, size_t>::type;
      using B = std::invoke_result<TB, size_t>::type;
      using U = decltype(std::declval<A>() * std::declval<B>());
      U sum = U(0);
      size_t len = a.Size();
      for (size_t k = 0; k < len; k++)
        sum += a(k) * b(k);
      return sum;
    }

    size_t Size() const { return 1; }
  };

  template <typename TA, typename TB>
  auto operator*(const VecExpr<TA> &a, const VecExpr<TB> &b)
  {
    return MultVecExpr(a.Upcast(), b.Upcast());
  }

  /*------------operator<<------------*/
  template <typename T>
  std::ostream &operator<<(std::ostream &ost, const VecExpr<T> &v)
  {
    if (v.Size() > 0)
      ost << v(0);
    for (size_t i = 1; i < v.Size(); i++)
      ost << ", " << v(i);
    return ost;
  }

}

#endif
