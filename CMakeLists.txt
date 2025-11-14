#cmake_minimum_required(VERSION 3.20)
#project(BLAga)
#
#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_BUILD_TYPE Release)
#
#
#set(CMAKE_OSX_ARCHITECTURES arm64)
#
## -----------------------------
## Python & pybind11 detection
## -----------------------------
#
#execute_process(
#    COMMAND which python3
#    OUTPUT_VARIABLE PYTHON_PATH
#    OUTPUT_STRIP_TRAILING_WHITESPACE
#)
#set(Python_EXECUTABLE "${PYTHON_PATH}")
#
#find_package(Python 3.8 COMPONENTS Interpreter Development REQUIRED)
#
#execute_process(
#    COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
#    OUTPUT_VARIABLE PYBIND11_DIR
#    OUTPUT_STRIP_TRAILING_WHITESPACE
#)
#
#list(APPEND CMAKE_PREFIX_PATH "${PYBIND11_DIR}")
#find_package(pybind11 CONFIG REQUIRED)
#message(STATUS "Found pybind11 at: ${PYBIND11_DIR}")
#
## -----------------------------
## Build Python extension
## -----------------------------
#pybind11_add_module(bla src/bind_bla.cpp)
#
#target_compile_options(bla PRIVATE
#    -O3
#    -mcpu=apple-m1
#    -ffast-math
#    -funroll-loops
#)
#
## -----------------------------
## Other includes / subdirs
## -----------------------------
#include_directories(src concurrentqueue amx cpp_demos)
#add_subdirectory(cpp_demos)
#
## -----------------------------
## Catch2 tests
## -----------------------------
#find_package(Catch2 3 QUIET)
#if(Catch2_FOUND)
#    enable_testing()
#    add_subdirectory(cpp_tests)
#    add_custom_target(run_tests ALL
#        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
#        DEPENDS cpp_tests
#    )
#endif()
#
## -----------------------------
## Install targets
## -----------------------------
#install(TARGETS bla DESTINATION BLAga)
#install(FILES 
#    src/vector.hpp 
#    src/matrix.hpp 
#    src/vectorexpression.hpp 
#    src/matrixexpression.hpp 
#    src/matrix_fwd.hpp
#    src/matrixexpressionspecialization.hpp
#    DESTINATION BLAga/include
#)
#
#
#
#
#
#
#

cmake_minimum_required(VERSION 3.20)
project(BLAga)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)

# Force ARM64 only, native CPU
#set(CMAKE_OSX_ARCHITECTURES arm64)

# Enable maximum optimization flags for Release
#set(CMAKE_CXX_FLAGS_RELEASE "-O1 -march=native -ffast-math -funroll-loops -flto -DNDEBUG")

# -----------------------------
# Python & pybind11 detection
# -----------------------------
find_package(Python 3 COMPONENTS Interpreter Development REQUIRED)

execute_process(
    COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
    OUTPUT_VARIABLE PYBIND11_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

list(APPEND CMAKE_PREFIX_PATH "${PYBIND11_DIR}")
find_package(pybind11 CONFIG REQUIRED)
message(STATUS "Found pybind11 at: ${PYBIND11_DIR}")

# -----------------------------
# Build Python extension
# -----------------------------
pybind11_add_module(bla src/bind_bla.cpp)

# Ensure Release flags propagate correctly
#target_compile_options(bla PRIVATE $<$<CONFIG:Release>: -O3 -march=native -ffast-math -funroll-loops -flto>)
set_target_properties(bla PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES)

# -----------------------------
# Other includes / subdirs
# -----------------------------
include_directories(src concurrentqueue amx cpp_demos)
add_subdirectory(cpp_demos)

# -----------------------------
# Catch2 tests
# -----------------------------
find_package(Catch2 3 QUIET)
if(Catch2_FOUND)
    enable_testing()
    add_subdirectory(cpp_tests)
    add_custom_target(run_tests ALL
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS cpp_tests
    )
endif()

# -----------------------------
# Install targets
# -----------------------------
#install(TARGETS bla DESTINATION BLAga)
install(TARGETS bla
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/BLAga
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/BLAga)
install(FILES 
    src/vector.hpp 
    src/matrix.hpp 
    src/vectorexpression.hpp 
    src/matrixexpression.hpp 
    src/matrix_fwd.hpp
    src/matrixexpressionspecialization.hpp
    DESTINATION BLAga/include
)
