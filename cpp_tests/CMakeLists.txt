# Create test executable
add_executable(cpp_tests
    test_vector.cpp
    test_matrix.cpp
)

# Link with your project’s library (or src)
target_link_libraries(cpp_tests
    PRIVATE
    Catch2::Catch2WithMain
    # your_project_lib  # if you built src as a library
)

# Register tests with CTest
include(Catch)
catch_discover_tests(cpp_tests)


