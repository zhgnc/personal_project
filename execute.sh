#!/bin/bash
set -euo pipefail 
# -e failure causes immeidate exit 
# -u unset variables cause errors 
# -o if any part of the pipeline fails the whole pipeline fails

info()    { echo -e "\e[1;4;32m$*\e[0m"; }   # Green, bold, underlined
error()   { echo -e "\e[1;4;31m$*\e[0m"; }   # Red, bold, underlined
warning() { echo -e "\e[1;33m$*\e[0m"; }     # Yellow, bold (optional)

# Get the absolute path to the script directory
ROOT_DIR=$(pwd)

# Set build directory relative to project root
BUILD_DIR="$ROOT_DIR\build"

# Create build directory if it does not exist 
mkdir -p "$BUILD_DIR"
echo "Using build directory: $BUILD_DIR"

# Clear configured files if they exist to improve build safety
if [ -f "$BUILD_DIR/CMakeCache.txt" ]; then
  echo "Build directory configured. Cleaning..."
  cmake --build "$BUILD_DIR" --target clean
else
  warning "No build configuration found. Skipping clean."
fi

# Generates make files and compiles project
cmake -G "Unix Makefiles" -S "$ROOT_DIR" -B "$BUILD_DIR" -DENABLE_TESTING=OFF

if cmake --build "$BUILD_DIR"; then
  info "Build Success!!!"
else
  error "Build Failed ❌"
  exit 1
fi

# Run the executable and check its exit status
echo && echo
EXECUTABLE="$BUILD_DIR/MyCode"

if [ ! -f "$EXECUTABLE" ]; then
  error "Executable Not Found. Cannot Run ❌"
  exit 1
fi

info "Running $EXECUTABLE"
if "$EXECUTABLE"; then
  info "Program Ran Successfully!"
else
  error "Program Failed During Execution. Runtime Error ❌"
  exit 1
fi