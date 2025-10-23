#include <iostream>

#include <matrix.hpp>

namespace bla = bla_ga;

void myfunc(bla::MatrixView<> A, bla::MatrixView<> B, bla::MatrixView<> C)
{
  C = A * B;
}

int main()
{
  //  size_t n = 3;
  //  bla::Matrix<double, bla::RowMajor> x(n, n);
  //  bla::Matrix<double, bla::ColMajor> y(n, n);
  //
  //  for (size_t i = 0; i < x.nRows(); i++)
  //    for (size_t j = 0; j < x.nCols(); j++)
  //    {
  //      x(i, j) = i;
  //      y(i, j) = 10 * j + 1;
  //    }
  //  std::cout << "x = " << x << std::endl;
  //  std::cout << "y = " << y << std::endl;
  //
  //  bla::Matrix<double, bla::RowMajor> z = 2 * x;
  //
  //  std::cout << "2*x = " << z << std::endl;
  //
  //  z = x + y;
  //
  //  std::cout << "x+y = " << z << std::endl;
  //  std::cout << "type of (x+3*y) is  " << typeid(x + 3 * y).name() << std::endl;
  //
  //  std::cout << "x+3*y = " << x + 3 * y << std::endl;
  //
  //  std::cout << "sizeof(x+3*y) = " << sizeof(x + 3 * y) << std::endl;
  //  std::cout << "x*y = " << x * y << std::endl;
  //  for (int iter = 0; iter < 10; iter++)
  //  {
  //    bla::Matrix<double, bla::RowMajor> result(x.nRows(), y.nCols());
  //    result = x * y;
  //    std::cout << "r = " << result << std::endl;
  //  }
  //
  //  // x.Range(2, 9) = 3;
  //  // x.Slice(1, 5) = 10;
}
