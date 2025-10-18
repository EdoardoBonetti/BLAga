# Welcome to BLAga's documentation!


BLAga is a C++ library for basic linear algebra operations.
The library provides template classes **Vector** and **Matrix**

## Installation

install it via git-clone:

    git clone --branch BLAga-Pybind https://github.com/EdoardoBonetti/BLAga

This branch of the repository contains the python bindings for BLAga

To install the python bindings run:

    python3 -m pip install .  

or, if you want to see more verbose output

    python3 -m pip install . -v   

In case you want to edit the python bindings, run

    python3 -m pip install -e . 

Or if you want to run tests 

    python3 -m pip install -e .[test]

You can combine the stuff

    python3 -m pip install -e . -v   

    

## Using BLAga

To use bla_ga in your code, set the compiler include path properly, and include the header files

    #include <vector.h>
    #include <matrix.h>

All objects are implemented in the namespace bla_ga. To use them with less typing, you can set

    namespace bla = bla_ga;

or even

    
    using namespace bla_ga;

    

You can create vectors and compute with vectors like:

                 
```cpp
Vector<double> x(5), y(5), z(5);
for (int i = 0; i < x.Size(); i++)
   x(i) = i;
y = 5.0
z = x+3*y;
cout << "z = " << z << endl;
```

For matrices you can choose between row-major (`RowMajor`) or column-major (`ColMajor`) storage,
default is row-major.

```cpp
Matrix<double,RowMajor> m1(5,3), m2(3,3);
for (int i = 0; i < m1.nCols(); i++)
  for (int j = 0; j < m1.nRows(); j++)
    m1(i,j) = i+j;
m2 = 3.7;
Matrix product = m1 * m2;
```

You can extract a row or a column from a matrix:

```cpp
Vector col1 = product.Col(1);
```


```cpp
Matrix cols12 = product.Cols(1,3);
```

some changes ...  

   
