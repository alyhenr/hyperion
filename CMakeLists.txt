cmake_minimum_required(VERSION 3.20)
project(Hyperion VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Hardcore compiler flags for performance and strictness
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror -O3 -march=native -flto")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# We will add subdirectories for tests and benchmarks later
# add_subdirectory(tests)
# add_subdirectory(benchmarks)

add_executable(hyperion src/main.cpp)