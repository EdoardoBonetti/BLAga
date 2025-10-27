cmake_minimum_required(VERSION 3.20)
project(BLAga)

set(CMAKE_CXX_STANDARD 20)

# -----------------------------
# General compile setup
# -----------------------------
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

include_directories(src concurrentqueue amx cpp_demos)

add_subdirectory(cpp_demos)

# -----------------------------
# Catch2 test support
# -----------------------------
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

# -----------------------------
# Python & pybind11 detection
# -----------------------------
# Force CMake to use the same Python as in your shell
execute_process(
    COMMAND which python3
    OUTPUT_VARIABLE PYTHON_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT EXISTS "${PYTHON_PATH}")
    message(FATAL_ERROR "Could not find python3 on your PATH.")
endif()
message(STATUS "Using system Python: ${PYTHON_PATH}")

set(Python_EXECUTABLE "${PYTHON_PATH}")
find_package(Python 3.8 COMPONENTS Interpreter Development REQUIRED)

# Show which Python we found
message(STATUS "Using Python interpreter: ${Python_EXECUTABLE}")
execute_process(
    COMMAND "${Python_EXECUTABLE}" --version
    OUTPUT_VARIABLE PY_VER
    ERROR_VARIABLE PY_VER_ERR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(PY_VER)
    message(STATUS "Python version detected: ${PY_VER}")
else()
    message(STATUS "Python version detected: ${PY_VER_ERR}")
endif()

# Check if pybind11 is installed in that Python environment
execute_process(
    COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
    OUTPUT_VARIABLE PYBIND11_DIR
    ERROR_VARIABLE PYBIND11_ERR
    RESULT_VARIABLE PYBIND11_RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT PYBIND11_RESULT EQUAL 0 OR PYBIND11_DIR STREQUAL "")
    message(FATAL_ERROR
        "pybind11 not found for Python at ${Python_EXECUTABLE}.\n"
        "To fix this, run:\n"
        "  ${Python_EXECUTABLE} -m pip install pybind11\n"
        "Then re-run CMake."
    )
endif()

# Add the path so CMake can find pybind11
list(APPEND CMAKE_PREFIX_PATH "${PYBIND11_DIR}")
find_package(pybind11 CONFIG REQUIRED)
message(STATUS "Found pybind11 at: ${PYBIND11_DIR}")

# -----------------------------
# Build Python extension
# -----------------------------
pybind11_add_module(bla src/bind_bla.cpp)

install(TARGETS bla DESTINATION BLAga)
install(FILES 
    src/vector.hpp 
    src/matrix.hpp 
    src/vectorexpression.hpp 
    src/matrixexpression.hpp 
    DESTINATION BLAga/include
)
