#ifndef FILE_VECTOR
#define FILE_VECTOR

#include <iostream>

#include "vectorexpression.hpp"

namespace bla_ga
{

  /*------------VectorView------------*/
  template <typename T, typename TDIST = std::integral_constant<size_t, 1>>
  class VectorView : public VecExpr<VectorView<T, TDIST>>
  {
  protected:
    T *data;
    size_t size;
    TDIST dist;

  public:
    VectorView() = default;
    VectorView(const VectorView &) = default;

    template <typename TDIST2>
    VectorView(const VectorView<T, TDIST2> &v2)
        : data(v2.Data()), size(v2.Size()), dist(v2.Dist()) {}

    VectorView(size_t _size, T *_data)
        : data(_data), size(_size) {}

    VectorView(size_t _size, TDIST _dist, T *_data)
        : data(_data), size(_size), dist(_dist) {}

    template <typename TB>
    VectorView &operator=(const VecExpr<TB> &v2)
    {
      for (size_t i = 0; i < size; i++)
        data[dist * i] = v2(i);
      return *this;
    }

    VectorView &operator=(T scal)
    {
      for (size_t i = 0; i < size; i++)
        data[dist * i] = scal;
      return *this;
    }

    T *Data() const { return data; }
    size_t Size() const { return size; }
    auto Dist() const { return dist; }

    T &operator()(size_t i) { return data[dist * i]; }
    const T &operator()(size_t i) const { return data[dist * i]; }

    auto Range(size_t first, size_t next) const
    {
      return VectorView(next - first, dist, data + first * dist);
    }

    auto Slice(size_t first, size_t slice) const
    {
      return VectorView<T, size_t>(size / slice, dist * slice, data + first * dist);
    }
  };

  /*------------Vector------------*/
  template <typename T>
  class Vector : public VectorView<T>
  {
    typedef VectorView<T> BASE;
    using BASE::data;
    using BASE::size;

  public:
    Vector(size_t size)
        : VectorView<T>(size, new T[size]) { ; }

    Vector(size_t size, const T *data)
        : VectorView<T>(size, new T[size])
    {
      for (size_t i = 0; i < size; i++)
        this->data[i] = data[i];
    }

    Vector(const Vector &v)
        : Vector(v.Size())
    {
      *this = v;
    }

    Vector(Vector &&v)
        : VectorView<T>(0, nullptr)
    {
      std::swap(size, v.size);
      std::swap(data, v.data);
    }

    template <typename TB>
    Vector(const VecExpr<TB> &v)
        : Vector(v.Size())
    {
      *this = v;
    }

    ~Vector() { delete[] data; }

    using BASE::operator=;
    Vector &operator=(const Vector &v2)
    {
      for (size_t i = 0; i < size; i++)
        data[i] = v2(i);
      return *this;
    }

    Vector &operator=(Vector &&v2)
    {
      std::swap(size, v2.size);
      std::swap(data, v2.data);
      return *this;
    }

    // Operator ==
    bool operator==(const Vector &v2) const
    {
      for (size_t i = 0; i < size; i++)
        if (data[i] != v2(i))
          return false;
      return true;
    }
  };

  /*------------operator<<------------*/
  template <typename... Args>
  std::ostream &operator<<(std::ostream &ost, const VectorView<Args...> &v)
  {
    if (v.Size() > 0)
      ost << v(0);
    for (size_t i = 1; i < v.Size(); i++)
      ost << ", " << v(i);
    return ost;
  }

}

#endif
