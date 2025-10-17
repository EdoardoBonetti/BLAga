cmake_minimum_required(VERSION 3.20)
project(bla_ga)

set (CMAKE_CXX_STANDARD 20)

include_directories(src)
add_subdirectory (demos)


# add optimization flags O3 and for math library
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 ")



# ---- Fetch Catch2 ----
include(FetchContent)
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.7.1 # Check for latest stable version
)
FetchContent_MakeAvailable(catch2)

enable_testing()
add_subdirectory(tests)
add_custom_target(run_tests ALL
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS tests  # the test executable(s)
)



