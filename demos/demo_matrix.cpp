#include <iostream>
#include <complex>
#include <vector.hpp>
#include <matrix.hpp>

namespace bla = bla_ga;

int main()
{
  // use colmaj as bla::ColMajor
  constexpr bla::ORDERING colmaj = bla::ColMajor;
  size_t n = 3;

  bla::Matrix<double, colmaj> x(n, n);

  x(0, 0) = 1;
  x(0, 1) = 2;
  x(0, 2) = 3;
  x(1, 0) = 4;
  x(1, 1) = 5;
  x(1, 2) = 6;
  x(2, 0) = 7;
  x(2, 1) = 8;
  x(2, 2) = 9;

  std::cout << x << std::endl;

  // use colmaj as bla::ColMajor
  constexpr bla::ORDERING rowmaj = bla::RowMajor;

  bla::Matrix<double, rowmaj> y(n, n);

  y(0, 0) = 1;
  y(1, 0) = 2;
  y(2, 0) = 3;
  y(0, 1) = 4;
  y(1, 1) = 5;
  y(2, 1) = 6;
  y(0, 2) = 7;
  y(1, 2) = 8;
  y(2, 2) = 9;

  std::cout << y << std::endl;

  // print flatten
  std::cout << x.Flatten() << std::endl;
  std::cout << y.Flatten() << std::endl;

  std::cout << std::endl;
  std::cout << y * x << std::endl;
  std::cout << x * y << std::endl;
}
