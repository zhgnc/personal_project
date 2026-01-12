cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(MyCode LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(ENABLE_TESTING "Enable unit tests" OFF)


include_directories(${PROJECT_SOURCE_DIR}/external/yaml-cpp/include)
link_directories(${PROJECT_SOURCE_DIR}/external/yaml-cpp/lib)

include_directories(${PROJECT_SOURCE_DIR}/external/hdf5/include)
link_directories(${PROJECT_SOURCE_DIR}/external/hdf5/lib)

add_compile_definitions(PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

# Build main executable only if tests are not enabled
if(NOT ENABLE_TESTING)
  file(GLOB_RECURSE SRC_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")
  add_executable(${PROJECT_NAME} ${SRC_FILES})
  target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

  target_link_libraries(${PROJECT_NAME} 
    yaml-cpp
    hdf5_cpp  
    hdf5) # The hdf5_cpp API calls the HDF5 C code (hdf5) so both libraries must be linked

  target_compile_options(${PROJECT_NAME} PRIVATE 
      -Wall                   # Common warnings
      -Wextra                 # Extra useful warnings
      -Werror                 # Treat warnings as errors 
      -pedantic               # Enforce strict ISO C++ compliance
      -Wshadow                # Warn about variable shadowing
      -Wformat=2              # More thorough printf/scanf format checks
      -fno-omit-frame-pointer # Better debug info and profiling
      -O1                     # Optimization level 1
      -g                      # Debug symbols
  )
endif()

# Enable tests only if requested
if(ENABLE_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()
