###############################
##### For Main Executable #####
###############################

cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(MyCode LANGUAGES C CXX)
add_executable(${PROJECT_NAME} src/main.cpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

# Add compile options (flags) to the target
target_compile_options(${PROJECT_NAME} PRIVATE 
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

######################
##### UNIT TESTS #####
######################

