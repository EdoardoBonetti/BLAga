add_executable (demo_vector demo_vector.cpp)
target_sources (demo_vector PUBLIC ../src/vector.hpp)

add_executable (demo_matrix demo_matrix.cpp)
target_sources (demo_matrix PUBLIC ../src/matrix.hpp)