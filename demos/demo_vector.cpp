#include <iostream>
#include <complex>

#include <vector.hpp>

namespace bla = bla_ga;

int main()
{
  using complex = std::complex<double>;
  size_t n = 5;

  complex c1(1, 2);
  complex c2(3, 4);
  bla::Vector<double> x(n);
  bla::Vector<complex> y(n);

  for (size_t i = 0; i < x.Size(); i++)
  {
    x(i) = i;
    y(i) = 10;
  }

  bla::Vector<complex> z = x + y;

  std::cout << "x+y = " << z << std::endl;

  bla::Vector<complex> w = c1 * x - c2 * y;
  std::cout << "w= " << w << std::endl;
}
