#include <iostream>

#include <vector.hpp>

namespace bla = bla_ga;

int main()
{
  size_t n = 5;
  bla::Vector<double> x(n), y(n);

  double mult = 0;

  for (size_t i = 0; i < x.Size(); i++)
  {
    x(i) = 1;
    y(i) = i;
    mult += x(i) * y(i);
  }

  bla::Vector<double> z = x + y;

  std::cout << "x+y = " << z << std::endl;

  std::cout << "type of (x+3*y) is  " << typeid(x + 3 * y).name() << std::endl;

  std::cout << "x+3*y = " << x + 3 * y << std::endl;

  std::cout << "sizeof(x+3*y) = " << sizeof(x + 3 * y) << std::endl;

  std::cout << "x*y = " << x * y << std::endl;
  std::cout << "x*y = " << x * y << std::endl;

  std::cout << "mult = " << mult << std::endl;

  bla::Vector<double> v(10);
  for (size_t i = 0; i < v.Size(); i++)
    v(i) = 10 * i;
  std::cout << "v = " << v << std::endl;

  v.Range(2, 9) = 3;
  v.Slice(1, 5) = 10;

  std::cout << "v = " << v << std::endl;
  std::cout << "v.Range(2, 9) = " << v.Range(2, 9) << std::endl;
  std::cout << "v.Slice(1, 5) = " << v.Slice(1, 5) << std::endl;
}
