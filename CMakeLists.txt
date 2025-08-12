cmake_minimum_required(VERSION 3.10..4.1 FATAL_ERROR)
project(MyCode LANGUAGES C CXX)
add_executable(MyCode src/main.cpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

## Require at least C++17 for this target
target_compile_features(MyCode PRIVATE cxx_std_17)

# Add compile options (flags) to the target
target_compile_options(MyCode PRIVATE 
    -Wall                   # Common warnings
    -Wextra                 # Extra useful warnings
    -Werror                 # Treat warnings as errors (enforce clean code)
    -pedantic               # Enforce strict ISO C++ compliance
    -Wshadow                # Warn about variable shadowing (common bug source)
    -Wformat=2              # More thorough printf/scanf format checks
    -fno-omit-frame-pointer # Better debug info and profiling
    -O0                     # Optimization level 0 (best for debugging)
    -g                      # Debug symbols
)

# Added src directory to included paths
target_include_directories(MyCode PRIVATE ${CMAKE_SOURCE_DIR}/src)