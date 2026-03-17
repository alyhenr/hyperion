cmake_minimum_required(VERSION 3.20)
project(Hyperion VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Hardcore compiler flags for performance and strictness
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror -O3 -march=native -flto")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

# We will add subdirectories for tests and benchmarks later
# add_subdirectory(tests)
# add_subdirectory(benchmarks)

add_executable(hyperion 
  src/main.cpp
  src/matching/OrderBook.cpp
)

# --- Testing Framework (Google Test) ---
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable CTest (CMake's test runner)
enable_testing()

# Uncomment this line so CMake knows to look in your tests/ folder
add_subdirectory(tests)