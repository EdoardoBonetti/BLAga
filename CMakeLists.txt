cmake_minimum_required(VERSION 3.20)
project(BLAga)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_subdirectory(cpp_demos)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# ---- Try to find system Catch2 ----
find_package(Catch2 3 QUIET)

if(Catch2_FOUND)
    message(STATUS "Catch2 found. Enabling tests...")
    enable_testing()
    add_subdirectory(cpp_tests)
    add_custom_target(run_tests ALL
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS cpp_tests
    )
else()
    message(WARNING "Catch2 not found. Skipping tests.")
endif()

# ---- Python & pybind11 ----
find_package(Python 3.8 COMPONENTS Interpreter Development REQUIRED)

execute_process(
    COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
    OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE PYBIND11_DIR
)
list(APPEND CMAKE_PREFIX_PATH "${PYBIND11_DIR}")
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(bla src/bind_bla.cpp)

install(TARGETS bla DESTINATION BLAga)
install(FILES 
    src/vector.hpp 
    src/matrix.hpp 
    src/vectorexpression.hpp 
    src/matrixexpression.hpp 
    DESTINATION BLAga/include
)
