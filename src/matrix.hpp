#ifndef FILE_MATRIX
#define FILE_MATRIX

#include <iostream>
#include "matrixexpression.hpp"
#include "matrix_fwd.hpp"

namespace bla_ga
{

  /*------------MatrixView------------*/
  template <typename T, ORDERING ORD, typename TDIST>
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
        return i * ncols + j;
      else
        return j * nrows + i;
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
      for (size_t j = 0; j < ncols; j++)
        for (size_t i = 0; i < nrows; i++)
          data[Index(i, j)] = 0;
    }

    MatrixView(size_t _nrows, size_t _ncols, TDIST _dist, T *_data)
        : data(_data), nrows(_nrows), ncols(_ncols), dist(_dist) {}

    template <typename TB>
    MatrixView &operator=(const MatExpr<TB> &m2)
    {
      if constexpr (requires { m2.Upcast().EvalSIMD(*this); })
        m2.Upcast().EvalSIMD(*this);
      else if constexpr (requires { m2.Upcast().Eval(*this); })
        m2.Upcast().Eval(*this);
      else
      {
        for (size_t j = 0; j < ncols; j++)
          for (size_t i = 0; i < nrows; i++)
            data[Index(i, j)] = m2(i, j);
      }
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

    template <typename TB>
    MatrixView &operator+=(const MatExpr<TB> &m2)
    {
      for (size_t j = 0; j < ncols; j++)
        for (size_t i = 0; i < nrows; i++)
          (*this)(i, j) += m2(i, j);
      return *this;
    }

    template <typename TB>
    MatrixView &operator-=(const MatExpr<TB> &m2)
    {
      for (size_t j = 0; j < ncols; j++)
        for (size_t i = 0; i < nrows; i++)
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

  /*------------Matrix------------*/
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
      if constexpr (ORD == RowMajor)
        return i * dist + j;
      else
        return j * dist + i;
    }

  public:
    Matrix(size_t nrows, size_t ncols)
        : MatrixView<T, ORD>(nrows, ncols, new T[nrows * ncols])
    {
      for (size_t i = 0; i < nrows * ncols; i++)
        data[i] = T(0);
    }

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
      for (size_t j = 0; j < ncols; j++)
        for (size_t i = 0; i < nrows; i++)
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

    // To create transpose Just swap nrows and ncols
    Matrix<T, ORD == RowMajor ? ColMajor : RowMajor> Transpose() const
    {
      return MatrixView<T, ORD == RowMajor ? ColMajor : RowMajor>(ncols, nrows, data);
    }

    // Create a functon Flatten that returns a vector
    Vector<T> Flatten() const { return Vector<T>(nrows * ncols, data); }

    // Create Row(i) and Col(j) functions
    VectorView<T> Row(size_t i) const
    {
      if constexpr (ORD == RowMajor)
        return VectorView<T>(ncols, data + i * ncols);
      else
        return VectorView<T>(nrows, data + i);
    }

    VectorView<T> Col(size_t j) const
    {
      if constexpr (ORD == RowMajor)
        return VectorView<T>(nrows, ncols, data + j);
      else
        return VectorView<T>(ncols, data + j * nrows);
    }
  };

  /*------------operator<<------------*/
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

} // namespace bla_ga

#endif