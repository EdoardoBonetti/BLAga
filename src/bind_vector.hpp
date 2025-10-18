#pragma once
#include <pybind11/pybind11.h>
#include <sstream>
#include "vector.hpp"

namespace py = pybind11;
using namespace bla_ga;

template <typename T>
void bind_vector(py::module &m, const char *classname)
{
    py::class_<Vector<T>>(m, classname)
        .def(py::init<size_t>(), py::arg("size"), "create vector of given size")
        .def("__len__", &Vector<T>::Size)
        .def("__getitem__", [](Vector<T> &self, int i)
             {
                 if (i < 0) i += self.Size();
                 if (i < 0 || i >= self.Size()) throw py::index_error("vector index out of range");
                 return self(i); })
        .def("__setitem__", [](Vector<T> &self, int i, T val)
             {
                 if (i < 0) i += self.Size();
                 if (i < 0 || i >= self.Size()) throw py::index_error("vector index out of range");
                 self(i) = val; })
        .def("__add__", [](Vector<T> &self, Vector<T> &other)
             { return Vector<T>(self + other); })
        .def("__mul__", [](Vector<T> &self, T scal)
             { return Vector<T>(scal * self); })
        .def("__rmul__", [](Vector<T> &self, T scal)
             { return Vector<T>(scal * self); })
        .def("__str__", [](const Vector<T> &self)
             {
                 std::stringstream str;
                 str << self;
                 return str.str(); });
}
