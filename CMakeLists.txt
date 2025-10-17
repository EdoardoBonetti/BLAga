cmake_minimum_required(VERSION 3.20)
project(ASC_bla)

set (CMAKE_CXX_STANDARD 20)

include_directories(src)

add_subdirectory (demos)

# add an O3 flag to all targets
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -g -O0")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")






