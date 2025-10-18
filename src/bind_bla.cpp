#include <pybind11/pybind11.h>
#include "bind_vector.hpp"
#include "bind_matrix.hpp"

namespace py = pybind11;

PYBIND11_MODULE(bla, m)
{
    m.doc() = "Basic linear algebra module";

    bind_vector<double>(m, "Vector");
    bind_matrix<double>(m, "Matrix");

    // Later you can also bind int, float, complex:
    bind_vector<int>(m, "VectorInt");
    // bind_matrix<std::complex<double>>(m, "MatrixCplx");
}
