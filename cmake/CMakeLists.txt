cmake_minimum_required(VERSION 3.10)
project(Code)
add_executable(Code ../src/main.cpp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_C_COMPILER "/usr/bin/gcc")       # adjust path as needed
set(CMAKE_CXX_COMPILER "/usr/bin/g++")     # adjust path as needed

## Require at least C++17 for this target
# target_compile_features(my_app PRIVATE cxx_std_17)