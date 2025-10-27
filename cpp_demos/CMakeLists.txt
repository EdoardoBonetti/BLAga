add_executable (demo_vector demo_vector.cpp)
add_executable (demo_matrix demo_matrix.cpp)
target_sources (demo_vector PUBLIC ../src/vector.hpp ../src/vectorexpression.hpp)
target_sources (demo_matrix PUBLIC ../src/matrix.hpp ../src/matrixexpression.hpp)
 

# add optimization flags O3 and for math library
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3  -lm")

add_executable (dummytest dummytest.cpp)
target_sources (dummytest PUBLIC ../src/matrix.hpp ../src/matrixexpression.hpp)
add_executable (demo_benchmark demo_benchmark.cpp)
target_sources (demo_benchmark PUBLIC ../src/matrix.hpp ../src/matrixexpression.hpp)

add_executable(demo_row_col demo_row_col.cpp)
target_sources (dummytest PUBLIC ../src/matrix.hpp ../src/matrixexpression.hpp)

add_executable(small_bench small_bench.cpp)
target_sources (small_bench PUBLIC ../src/matrix.hpp ../src/matrixexpression.hpp)


add_executable(benchmark benchmark.cpp)
target_sources (benchmark PUBLIC ../src/matrix.hpp ../src/matrixexpression.hpp)


add_executable(tune_gemm tune_gemm.cpp)
target_sources (tune_gemm PUBLIC ../src/matrix.hpp ../src/matrixexpression.hpp)


add_executable(tune_simd tune_simd.cpp)
target_sources (tune_simd PUBLIC ../src/matrix.hpp ../src/matrixexpression.hpp ../src/simd.h)
