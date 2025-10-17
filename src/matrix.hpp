#ifndef FILE_MATRIX
#define FILE_MATRIX

#include <iostream>

#include "matrixexpression.hpp"

namespace bla_ga
{
  enum ORDERING
  {
    RowMajor,
    ColMajor
  };

  template <typename T, ORDERING ORD, typename TDIST = std::integral_constant<size_t, 1>>
  class MatrixView : public MatExpr<MatrixView<T, ORD, TDIST>>
  {
  protected:
    T *data;
    size_t nrows;
    size_t ncols;
    TDIST dist;

    constexpr size_t Index(size_t i, size_t j) const noexcept
    {
      if constexpr (ORD == RowMajor)
        return i * ncols + j; // row-major contiguous
      else
        return j * nrows + i; // column-major contiguous
    }

  public:
    MatrixView() = default;
    MatrixView(const MatrixView &) = default;

    template <typename TDIST2, ORDERING ORD2>
    MatrixView(const MatrixView<T, ORD2, TDIST2> &m2)
        : data(m2.Data()), nrows(m2.nRows()), ncols(m2.nCols()), dist(m2.Dist()) {}

    MatrixView(size_t _nrows, size_t _ncols, T *_data)
        : data(_data), nrows(_nrows), ncols(_ncols)
    {
      dist = std::integral_constant<size_t, 1>();
    }

    MatrixView(size_t _nrows, size_t _ncols, TDIST _dist, T *_data)
        : data(_data), nrows(_nrows), ncols(_ncols), dist(_dist) {}

    template <typename TB>
    MatrixView &operator=(const MatExpr<TB> &m2)
    {
      for (size_t i = 0; i < nrows; i++)
        for (size_t j = 0; j < ncols; j++)
          data[Index(i, j)] = m2(i, j);
      return *this;
    }

    T *Data() const { return data; }
    size_t nCols() const { return ncols; }
    size_t nRows() const { return nrows; }
    auto Dist() const { return dist; }
    T &operator()(size_t i, size_t j)
    {
      return data[Index(i, j)];
    }

    const T &operator()(size_t i, size_t j) const
    {
      return data[Index(i, j)];
    }

    VectorView<T, TDIST> Flattened() const
    {
      return VectorView<T, TDIST>(nrows * ncols, data);
    }

    template <typename TB>
    MatrixView &operator+=(const MatExpr<TB> &m2)
    {
      for (size_t i = 0; i < nrows; i++)
        for (size_t j = 0; j < ncols; j++)
          (*this)(i, j) += m2(i, j);
      return *this;
    }

    template <typename TB>
    MatrixView &operator-=(const MatExpr<TB> &m2)
    {
      for (size_t i = 0; i < nrows; i++)
        for (size_t j = 0; j < ncols; j++)
          (*this)(i, j) -= m2(i, j);
      return *this;
    }

    MatrixView &operator*=(T scal)
    {
      for (size_t i = 0; i < nrows; i++)
        for (size_t j = 0; j < ncols; j++)
          (*this)(i, j) *= scal;
      return *this;
    }
  };

  template <typename T, ORDERING ORD>
  class Matrix : public MatrixView<T, ORD>
  {
    typedef MatrixView<T, ORD> BASE;
    using BASE::data;
    using BASE::dist;
    using BASE::ncols;
    using BASE::nrows;

  protected:
    constexpr size_t Index(size_t i, size_t j) const noexcept
    {
      return data->Index(i, j);
    }

  public:
    Matrix(size_t nrows, size_t ncols)
        : MatrixView<T, ORD>(nrows, ncols, new T[nrows * ncols]) {}

    Matrix(const Matrix &m)
        : Matrix(m.nRows(), m.nCols())
    {
      *this = m;
    }

    Matrix(Matrix &&m)
        : MatrixView<T, ORD>(0, 0, nullptr)
    {
      std::swap(nrows, m.nrows);
      std::swap(ncols, m.ncols);
      std::swap(data, m.data);
    }

    template <typename TB>
    Matrix(const MatExpr<TB> &m)
        : Matrix(m.nRows(), m.nCols())
    {
      *this = m;
    }

    ~Matrix() { delete[] data; }

    using BASE::operator=;
    Matrix &operator=(const Matrix &m2)
    {

      size_t dist = nrows;
      for (size_t i = 0; i < nrows; i++)
        for (size_t j = 0; j < ncols; j++)
          data[Index(i, j)] = m2(i, j);
      return *this;
    }

    Matrix &operator=(Matrix &&m2)
    {
      std::swap(nrows, m2.nrows);
      std::swap(ncols, m2.ncols);
      std::swap(data, m2.data);
      return *this;
    }
  };

  template <typename... Args>
  std::ostream &operator<<(std::ostream &ost, const MatrixView<Args...> &m)
  {
    if (m.nRows() > 0 && m.nCols() > 0)
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

} // namespace ASC_bla

#endif