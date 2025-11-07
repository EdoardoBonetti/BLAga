// matrix_fwd.hpp
#ifndef FILE_MATRIX_FWD
#define FILE_MATRIX_FWD

#include <cstddef>
#include <type_traits>

namespace bla_ga
{

    enum ORDERING
    {
        RowMajor,
        ColMajor
    };

    template <typename T = double, ORDERING ORD = RowMajor>
    class MatrixView;

    template <typename T = double, ORDERING ORD = RowMajor>
    class Matrix;

} // namespace bla_ga

#endif
