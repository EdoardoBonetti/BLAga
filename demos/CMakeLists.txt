add_executable (demo_vector demo_vector.cpp)
add_executable (demo_matrix demo_matrix.cpp)
add_executable (dummytest dummytest.cpp)
add_executable (demo_benchmark demo_benchmark.cpp)


target_sources (demo_vector PUBLIC ../src/vector.hpp ../src/vectorexpression.hpp)
target_sources (demo_matrix PUBLIC ../src/matrix.hpp ../src/matrixexpression.hpp)
target_sources (dummytest PUBLIC ../src/matrix.hpp ../src/matrixexpression.hpp)
target_sources (demo_benchmark PUBLIC ../src/matrix.hpp ../src/matrixexpression.hpp)
 
# Enable AddressSanitizer only for this target
#target_compile_options(demo_benchmark PRIVATE -fsanitize=address -g)
#target_link_options(demo_benchmark PRIVATE -fsanitize=address)

