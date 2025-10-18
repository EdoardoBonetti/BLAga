#pragma once
#include <pybind11/pybind11.h>
#include <sstream>
#include "matrix.hpp"

namespace py = pybind11;
using namespace bla_ga;

template <typename T>
void bind_matrix(py::module &m, const char *classname)
{
    py::class_<Matrix<T>>(m, classname)
        .def(py::init<size_t, size_t>(), py::arg("rows"), py::arg("cols"))
        .def("nRows", &Matrix<T>::nRows)
        .def("nCols", &Matrix<T>::nCols)
        .def("__getitem__", [](Matrix<T> &m, std::pair<int, int> idx)
             {
                 int i = idx.first < 0 ? idx.first + m.nRows() : idx.first;
                 int j = idx.second < 0 ? idx.second + m.nCols() : idx.second;
                 return m(i,j); })
        .def("__setitem__", [](Matrix<T> &m, std::pair<int, int> idx, T val)
             {
                 int i = idx.first < 0 ? idx.first + m.nRows() : idx.first;
                 int j = idx.second < 0 ? idx.second + m.nCols() : idx.second;
                 m(i,j) = val; })
        .def("__add__", [](Matrix<T> &self, Matrix<T> &other)
             { return Matrix<T>(self + other); })

        .def("__mul__", [](Matrix<T> &self, py::object other)
             {
            if (py::isinstance<Matrix<T>>(other))
            {
                Matrix<T> &m = other.cast<Matrix<T>&>();
                return Matrix<T>(self * m); // matrix multiplication
            }
            else
            {
                T scal = other.cast<T>();
                return Matrix<T>(scal * self); // scalar multiplication
            } })
        .def("__rmul__", [](Matrix<T> &self, py::object other)
             {
            if (py::isinstance<Matrix<T>>(other))
            {
                Matrix<T> &m = other.cast<Matrix<T>&>();
                return Matrix<T>(m * self);
            }
            else
            {
                T scal = other.cast<T>();
                return Matrix<T>(scal * self);
            } })

        .def("__mul__", [](Matrix<T> &self, Matrix<T> &other)
             { return Matrix<T>(self * other); })

        .def("__rmul__", [](Matrix<T> &self, Matrix<T> &other)
             { return Matrix<T>(self * other); })

        .def("__str__", [](const Matrix<T> &m)
             {
                 std::stringstream ss;
                 ss << m;
                 return ss.str(); });
}
