#!/bin/bash

# Get the absolute path to the script directory
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# If on Cygwin or Git Bash, convert to Windows-native path
if command -v cygpath &> /dev/null; then
  ROOT_DIR="$(cygpath -w "$ROOT_DIR")"
fi

# Set debug-specific build directory relative to project root
BUILD_DIR="$ROOT_DIR/build/debug"

# Create debug-specific build directory if it does not exist
if [ -d "$BUILD_DIR" ]; then
  echo "Debug-specific build directory exists."
else
  echo "Build-specific directory does not exist. Creating directory..."
  mkdir -p "$BUILD_DIR"
fi

# Clear previous configuration if it exists
if [ -f "$BUILD_DIR/CMakeCache.txt" ]; then
  echo "Debyug-specific build directory already configured. Cleaning..."
  cmake --build "$BUILD_DIR" --target clean
else
  echo "No previous configuration found. Skipping clean."
fi

# Run CMake with Debug configuration
echo "Configuring project in Debug mode..."
cmake -G "Unix Makefiles" -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug

# Compile the project
echo "Building debug-specific project..."
if cmake --build "$BUILD_DIR"; then
  echo -e "\e[1;4;32mDebug-Specific Build Success!!!\e[0m"
else
  echo -e "\e[1;4;31mDebug-Specific Build Failed ❌\e[0m"
  exit 1
fi

# Path to the executable (adjust to match your target name)
EXECUTABLE="$BUILD_DIR/MyCode"

# Run the debugger if the executable exists
echo && echo
if [ -f "$EXECUTABLE" ]; then
  echo -e "\nLaunching debugger for $EXECUTABLE\n"
  gdb "$EXECUTABLE"
else
  echo -e "\e[1;4;31mExecutable Not Found. Cannot Run ❌\e[0m"
  exit 1
fi
