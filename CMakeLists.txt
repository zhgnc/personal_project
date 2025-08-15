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
add_compile_definitions(_POSIX_C_SOURCE=200809L)

set(GTEST_DIR "${PROJECT_SOURCE_DIR}/external/googletest")
add_subdirectory(${GTEST_DIR} ${CMAKE_BINARY_DIR}/googletest_build)

enable_testing()

file(GLOB TEST_SOURCES "${PROJECT_SOURCE_DIR}/tests/*.cpp")

foreach(test_source IN LISTS TEST_SOURCES)
  get_filename_component(test_name ${test_source} NAME_WE)
  add_executable(${test_name} ${test_source})
  target_link_libraries(${test_name} PRIVATE gtest gtest_main)
  target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
  add_test(NAME ${test_name} COMMAND ${test_name} --gtest_verbose=info)
endforeach()