#ifndef FILE_MATRIX
#define FILE_MATRIX

#include <iostream>

namespace bla_ga
{

  enum ORDERING
  {
    ColMajor,
    RowMajor
  };

  constexpr ORDERING result_ordering_sum(ORDERING a, ORDERING b)
  {
    return a;
  }

  constexpr ORDERING result_ordering_mul(ORDERING a, ORDERING b)
  {
    return (a == b) ? a : (a == RowMajor && b == ColMajor) ? RowMajor
                      : (a == ColMajor && b == RowMajor)   ? ColMajor
                                                           : a; // fallback
  }

  template <typename T, ORDERING ORD>
  class Matrix
  {

    size_t nrows;
    size_t ncols;
    T *data;
    // ORDERING ordering;

  public:
    Matrix(size_t nrows, size_t ncols) : nrows(nrows), ncols(ncols), data(new T[nrows * ncols]) {}

    Matrix(const Matrix &m) : nrows(m.nrows), ncols(m.ncols)
    {
      *this = m;
    }
    Matrix(Matrix &&m) : nrows(0), ncols(0)
    {
      std::swap(nrows, m.nrows);
      std::swap(ncols, m.ncols);
      std::swap(data, m.data);
    }
    ~Matrix() { delete[] data; }

    Matrix &operator=(const Matrix &m2)
    {
      for (size_t i = 0; i < nrows; i++)
        for (size_t j = 0; j < ncols; j++)
          (*this)(i, j) = m2(i, j);
      return *this;
    }

    Matrix &operator=(Matrix &&m2)
    {
      std::swap(nrows, m2.nrows);
      std::swap(ncols, m2.ncols);
      std::swap(data, m2.data);
      return *this;
    }

    size_t nCols() const { return ncols; }
    size_t nRows() const { return nrows; }
    Vector<T> Shape() const
    {
      const Vector<T> shape(2);
      shape(0) = nrows;
      shape(1) = ncols;
      return shape;
    }

    T &operator()(size_t i, size_t j)
    {
      if constexpr (ordering == ColMajor)
        return data[i * ncols + j];
      else
        return data[j * nrows + i];
    }

    const T &operator()(size_t i, size_t j) const
    {
      if (ordering == ColMajor)
        return data[i * ncols + j];
      else
        return data[j * nrows + i];
    }

    // Flatten returning data as vector
    Vector<T> Flatten() const
    {
      Vector<T> flat(nrows * ncols);
      for (size_t i = 0; i < nrows * ncols; i++)
        flat(i) = data[i];
      return flat;
    }
  };

  template <typename T, typename S, ORDERING ORDT, ORDERING ORDS>
  Matrix<decltype(std::declval<T>() + std::declval<S>()), result_ordering_sum(ORDT, ORDS)>
  operator+(const Matrix<T, ORDT> &a, const Matrix<S, ORDS> &b)
  {
    typedef decltype(std::declval<T>() + std::declval<S>()) U;
    constexpr ORDERING ORDU = result_ordering_sum(ORDT, ORDS);
    Matrix<U, ORDU> sum(a.nRows(), a.nCols());
    for (size_t i = 0; i < a.nRows(); i++)
      for (size_t j = 0; j < a.nCols(); j++)
        sum(i, j) = a(i, j) + b(i, j);
    return sum;
  }

  // Multiply by scalar
  template <typename T, ORDERING ORD, typename S>
  Matrix<decltype(std::declval<T>() * std::declval<S>()), ORD> operator*(const Matrix<T, ORD> &a, const S &s)
  {
    typedef decltype(std::declval<T>() * std::declval<S>()) U;
    constexpr ORDERING ORDU = ORD;

    Matrix<U, ORDU> sum(a.nRows(), a.nCols());
    for (size_t i = 0; i < a.nRows(); i++)
      for (size_t j = 0; j < a.nCols(); j++)
        sum(i, j) = a(i, j) * s;
    return sum;
  }

  // by matrix
  template <typename T, typename S, ORDERING ORDT, ORDERING ORDS>
  Matrix<decltype(std::declval<T>() * std::declval<S>()), result_ordering_mul(ORDT, ORDS)>
  operator*(const Matrix<T, ORDT> &a, const Matrix<S, ORDS> &b)
  {
    typedef decltype(std::declval<T>() * std::declval<S>()) U;
    constexpr ORDERING ORDU = result_ordering_mul(ORDT, ORDS);
    Matrix<U, ORDU> sum(a.nRows(), b.nCols());
    for (size_t i = 0; i < a.nRows(); i++)
      for (size_t j = 0; j < b.nCols(); j++)
        for (size_t k = 0; k < a.nCols(); k++)
          sum(i, j) = sum(i, j) + a(i, k) * b(k, j);
    return sum;
  }

  template <typename T, ORDERING ORD>
  std::ostream &operator<<(std::ostream &ost, const Matrix<T, ORD> &m)
  {
    for (size_t i = 0; i < m.nRows(); i++)
    {
      for (size_t j = 0; j < m.nCols(); j++)
        ost << m(i, j) << " ";
      ost << std::endl;
    }
    return ost;
  }

}

#endif
