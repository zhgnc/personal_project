#!/bin/bash

# Get the absolute path to the script directory
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# If on Cygwin or Git Bash, convert to Windows-native path
if command -v cygpath &> /dev/null; then
  ROOT_DIR="$(cygpath -w "$ROOT_DIR")"
fi

# Set build directory relative to project root
BUILD_DIR="$ROOT_DIR/build"

# Create build directory if it does not exist 
if [ -d "$BUILD_DIR" ]; then
  echo "Build directory exists."
else
  echo "Build directory does not exists. Making directory."
  mkdir -p "$BUILD_DIR"
fi 

# Clear configured files if they exist to improve build safety
if [ -f "$BUILD_DIR/CMakeCache.txt" ]; then
  echo "Build directory configured. Cleaning..."
  cmake --build "$BUILD_DIR" --target clean
else
  echo "No build configuration found. Skipping clean."
fi

# Generates make files and compiles project
cmake -G "Unix Makefiles" -S "$ROOT_DIR" -B "$BUILD_DIR"
if cmake --build "$BUILD_DIR"; then
  echo -e "\e[1;4;32mBuild Success!!!\e[0m" # Odd characters make the text green, bolded, and underlined
else
  echo -e "\e[1;4;31mBuild Failed ❌\e[0m"
  exit 1
fi

# Run the executable and check its exit status
echo && echo
EXECUTABLE="$BUILD_DIR/MyCode"

if [ -f "$EXECUTABLE" ]; then
  echo -e "\nRunning $EXECUTABLE\n"
  if "$EXECUTABLE"; then
    echo -e "\e[1;4;32mProgram Ran Successfully!\e[0m"
  else
    echo -e "\e[1;4;31mProgram Failed During Execution. Runtime Error ❌\e[0m"
    exit 1
  fi
else
  echo -e "\e[1;4;31mExecutable Not Found. Cannot Run ❌\e[0m"
  exit 1
fi