#!/bin/bash

# Get the absolute path to the script directory
ROOT_DIR=$(pwd)

# Set build directory relative to project root
BUILD_DIR="$ROOT_DIR\build"

# Create build directory if it does not exist 
mkdir -p "$BUILD_DIR"
echo "Using build directory: $BUILD_DIR"

# Clean build if cache exists
if [ -f "$BUILD_DIR/CMakeCache.txt" ]; then
  echo "Previous build detected. Cleaning..."
  cmake --build "$BUILD_DIR" --target clean
else
  echo "No previous build config. Skipping clean."
fi

# Run CMake configuration
echo "Configuring project..."
cmake -G "Unix Makefiles" -S "$ROOT_DIR" -B "$BUILD_DIR" -DENABLE_TESTING=ON

# Build project
echo "Building project..."
if cmake --build "$BUILD_DIR"; then
  echo -e "\e[1;32m✅ Build succeeded!\e[0m"
else
  echo -e "\e[1;31m❌ Build failed.\e[0m"
  exit 1
fi

# Run tests
echo -e "\nRunning tests..."
cd "$BUILD_DIR/tests"
echo "Running ctest from directory: $(pwd)"
ctest --output-on-failure