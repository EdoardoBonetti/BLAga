#ifndef FILE_MATRIX
#define FILE_MATRIX

#include <iostream>
#include "vector.hpp"

namespace bla_ga
{

  enum ORDERING
  {
    ColMajor,
    RowMajor
  };

  template <typename T = double, ORDERING ORD = RowMajor>
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

    Matrix(Matrix &&m) noexcept : nrows(0), ncols(0), data(nullptr)
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
      if constexpr (ORD == ColMajor)
        return data[i * ncols + j];
      else
        return data[j * nrows + i];
    }
    const T &operator()(size_t i, size_t j) const
    {
      if constexpr (ORD == ColMajor)
        return data[i * ncols + j];
      else
        return data[j * nrows + i];
    }

    Vector<T> Col(size_t j)
    {
      Vector<T> col(nrows);
      for (size_t i = 0; i < nrows; i++)
        col(i) = (*this)(i, j);
      return col;
    }

    Vector<T> Row(size_t i)
    {
      Vector<T> row(ncols);
      for (size_t j = 0; j < ncols; j++)
        row(j) = (*this)(i, j);
      return row;
    }

    // Flatten returning data as vector
    Vector<T> Flatten() const
    {
      Vector<T> flat(nrows * ncols);
      for (size_t i = 0; i < nrows * ncols; i++)
        flat(i) = data[i];
      return flat;
    }

    // Transpose
    Matrix<T, ORD == ColMajor ? RowMajor : ColMajor> Transpose() const
    {
      Matrix<T, ORD == ColMajor ? RowMajor : ColMajor> trans(ncols, nrows);
      for (size_t i = 0; i < nrows; i++)
        for (size_t j = 0; j < ncols; j++)
          trans(j, i) = (*this)(i, j);
      return trans;
    }

    void DownwardElimination();
    void UpwardElimination();
    Matrix<T, ORD> GaussianElimination();

    Matrix<T, ORD> Rows(size_t start, size_t end, size_t step = 1) const;
    Matrix<T, ORD> Cols(size_t start, size_t end, size_t step = 1) const;
  };

  template <typename T, typename S, ORDERING ORDT, ORDERING ORDS>
  Matrix<decltype(T{} + S{}), ORDT>
  operator+(const Matrix<T, ORDT> &a, const Matrix<S, ORDS> &b)
  {
    using U = decltype(T{} + S{});
    constexpr ORDERING ORDU = ORDT;

    Matrix<U, ORDU> sum(a.nRows(), a.nCols());
    for (size_t i = 0; i < a.nRows(); i++)
      for (size_t j = 0; j < a.nCols(); j++)
        sum(i, j) = a(i, j) + b(i, j);
    return sum;
  }

  // Multiply by scalar
  template <typename T, ORDERING ORD, typename S>
  Matrix<decltype(T{} * S{}), ORD> operator*(const Matrix<T, ORD> &a, const S &s)
  {
    typedef decltype(T{} * S{}) U;
    constexpr ORDERING ORDU = ORD;

    Matrix<U, ORDU> sum(a.nRows(), a.nCols());
    for (size_t i = 0; i < a.nRows(); i++)
      for (size_t j = 0; j < a.nCols(); j++)
        sum(i, j) = a(i, j) * s;
    return sum;
  }

  // by matrix
  template <typename T, typename S, ORDERING ORDT, ORDERING ORDS>
  Matrix<decltype(T{} * S{}), ORDT>
  operator*(const Matrix<T, ORDT> &a, const Matrix<S, ORDS> &b)
  {
    using U = decltype(T{} * S{});
    constexpr ORDERING ORDU = ORDT;
    Matrix<U, ORDU> sum(a.nRows(), b.nCols());
    for (size_t i = 0; i < a.nRows(); i++)
      for (size_t j = 0; j < b.nCols(); j++)
        for (size_t k = 0; k < a.nCols(); k++)
          sum(i, j) = sum(i, j) + a(i, k) * b(k, j);
    return sum;
  }

  template <typename T, ORDERING ORDT, typename S>
  Vector<decltype(T{} * S{})>
  operator*(const Matrix<T, ORDT> &m, const Vector<S> &v)
  {
    using U = decltype(T{} * S{});
    Vector<U> sum(m.nRows());
    for (size_t i = 0; i < m.nRows(); i++)
      for (size_t j = 0; j < m.nCols(); j++)
        sum(i) = sum(i) + m(i, j) * v(j);
    return sum;
  }

  template <typename T, ORDERING ORDT, typename S>
  Vector<decltype(T{} * S{})>
  operator*(const Vector<S> &v, const Matrix<T, ORDT> &m)
  {
    using U = decltype(T{} * S{});
    Vector<U> sum(m.nCols());
    for (size_t i = 0; i < m.nRows(); i++)
      for (size_t j = 0; j < m.nCols(); j++)
        sum(j) = sum(j) + m(i, j) * v(i);
    return sum;
  }

  // glue horizontal
  template <typename T, ORDERING ORDT, typename S, ORDERING ORDS>
  Matrix<decltype(T{} * S{}), ORDT>
  HGlue(const Matrix<T, ORDT> &a, const Matrix<S, ORDS> &b)
  {
    using U = decltype(T{} * S{});
    constexpr ORDERING ORDU = ORDT;
    Matrix<U, ORDU> ab(a.nRows(), a.nCols() + b.nCols());
    for (size_t i = 0; i < a.nRows(); i++)
    {
      for (size_t j = 0; j < a.nCols(); j++)
      {
        ab(i, j) = a(i, j);
      }
      for (size_t j = 0; j < b.nCols(); j++)
      {
        ab(i, j + a.nCols()) = b(i, j);
      }
    }
    return ab;
  }
  template <typename T, ORDERING ORDT, typename S, ORDERING ORDS>
  Matrix<decltype(T{} * S{}), ORDT>
  VGlue(const Matrix<T, ORDT> &a, const Matrix<S, ORDS> &b)
  {
    using U = decltype(T{} * S{});
    constexpr ORDERING ORDU = ORDT;
    Matrix<U, ORDU> ab(a.nRows() + b.nRows(), a.nCols());
    for (size_t i = 0; i < a.nRows(); i++)
    {
      for (size_t j = 0; j < a.nCols(); j++)
      {
        ab(i, j) = a(i, j);
      }
    }
    for (size_t i = 0; i < b.nRows(); i++)
    {
      for (size_t j = 0; j < b.nCols(); j++)
      {
        ab(i + a.nRows(), j) = b(i, j);
      }
    }
    return ab;
  }

  template <typename T, ORDERING ORD>
  Matrix<T, ORD> Eye(size_t n)
  {
    Matrix<T, ORD> id(n, n);
    for (size_t i = 0; i < n; i++)
      id(i, i) = 1;
    return id;
  }

  template <typename T, ORDERING ORD>
  Matrix<T, ORD> Diag(Vector<T> v)
  {
    Matrix<T, ORD> diag(v.size(), v.size());
    for (size_t i = 0; i < v.size(); i++)
      diag(i, i) = v(i);
    return diag;
  }

  template <typename T, ORDERING ORD>
  Vector<T> Diag(Matrix<T, ORD> m)
  {
    Vector<T> diag(m.nRows());
    for (size_t i = 0; i < m.nRows(); i++)
      diag(i) = m(i, i);
    return diag;
  }

  template <typename T, ORDERING ORD>
  void Matrix<T, ORD>::DownwardElimination()
  {
    for (size_t i = 0; i < nrows; i++) // Pivot row
    {
      T pivot = (*this)(i, i);

      // Optional: check for near-zero pivot
      if (std::abs(pivot) < 1e-12)
        throw std::runtime_error("Zero or near-zero pivot encountered");

      for (size_t j = i + 1; j < nrows; j++) // Eliminate below pivot
      {
        T factor = (*this)(j, i) / pivot; // Store multiplier temporarily

        for (size_t k = i; k < ncols; k++)
          (*this)(j, k) -= factor * (*this)(i, k);
      }
    }
  }

  template <typename T, ORDERING ORD>
  void Matrix<T, ORD>::UpwardElimination()
  {
    // Work upward from the last pivot to the first
    for (int i = int(nrows) - 1; i >= 0; i--)
    {
      T pivot = (*this)(i, i);

      // Check for zero or tiny pivot to prevent division by zero
      if (std::abs(pivot) < 1e-12)
        throw std::runtime_error("Zero or near-zero pivot encountered in UpwardElimination");

      // Normalize pivot row
      for (size_t k = 0; k < ncols; k++)
        (*this)(i, k) /= pivot;

      // Eliminate entries above the pivot
      for (int j = i - 1; j >= 0; j--)
      {
        T factor = (*this)(j, i);
        for (size_t k = 0; k < ncols; k++)
          (*this)(j, k) -= factor * (*this)(i, k);
      }
    }
  }

  template <typename T, ORDERING ORD>
  Matrix<T, ORD> Matrix<T, ORD>::Rows(size_t start, size_t end, size_t step) const
  {
    size_t outRows = (end - start + step - 1) / step;
    Matrix<T, ORD> result(ncols, outRows);
    for (size_t j = 0; j < ncols; j++)
      for (size_t i = start, ii = 0; i < end; i += step, ii++)
        result(ii, j) = (*this)(i, j);
    return result;
  }
  template <typename T, ORDERING ORD>
  Matrix<T, ORD> Matrix<T, ORD>::Cols(size_t start, size_t end, size_t step) const
  {
    size_t outCols = (end - start + step - 1) / step;
    Matrix<T, ORD> result(nrows, outCols);
    for (size_t i = 0; i < nrows; i++)
      for (size_t j = start, jj = 0; j < end; j += step, jj++)
        result(i, jj) = (*this)(i, j);
    return result;
  }

  template <typename T, ORDERING ORD>
  Matrix<T, ORD> GaussElimination(const Matrix<T, ORD> &m)
  {
    // Create identity matrix of same size
    auto id = Eye<T, ORD>(m.nRows());

    // Horizontally glue original + identity
    auto temp = HGlue<T, ORD, T, ORD>(m, id);

    temp.DownwardElimination();

    temp.UpwardElimination();

    // Return a copy
    return temp.Cols(m.nCols(), 2 * m.nCols());
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
