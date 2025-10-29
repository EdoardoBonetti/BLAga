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
         // New command maybe take away
         .def("__setitem__", [](Vector<T> &v, py::slice slice, T value)
              {
                   size_t start, stop, step, slicelength;
                   if (!slice.compute(v.Size(), &start, &stop, &step, &slicelength))
                        throw py::error_already_set();

                   // Create a VectorView of the slice
                   auto view = v.SubVector(start, stop, step); // you already defined SubVector
                   view = value;                               // uses the C++ operator= for VectorView
              })
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

template <typename T>
void bind_vectorview(py::module &m, const char *classname)
{
     py::class_<VectorView<T>>(m, classname)
         .def("__len__", &VectorView<T>::Size)
         .def("__getitem__", [](VectorView<T> &v, size_t i)
              {
            if (i >= v.Size()) throw py::index_error();
            return v(i); })
         .def("__setitem__", [](VectorView<T> &v, size_t i, T val)
              {
            if (i >= v.Size()) throw py::index_error();
            v(i) = val; })
         .def("__repr__", [](const VectorView<T> &v)
              {
            std::ostringstream ss;
            ss << "\n";
            for (size_t i = 0; i < v.Size(); ++i)
            {
                if (i > 0) ss << ", ";
                ss << v(i);
            }
            ss << "\n";
            return ss.str(); });
}
