#!/bin/bash

# Create build directory if it does not exist 
if [ -d build ]; then
  echo "Build directory exists."
else
  echo "Build directory does not exists. Making directory."
  mkdir build
fi 

cd build

# Clear configured files if they exist to improve build safety
if [-f CMakeCache.txt ]; then
  echo "Build directory configured. Cleaning..."
  cmake --build . --target clean
else
  echo "No build configuration found. Skipping clean."
fi

# Generates make files and compiles project
cmake -G "Unix Makefiles" .. 
cmake --build .
echo -e "\e[1;4;32mBuild Success!!!\e[0m" # Odd characters make the text green, bolded, and underlined

# Run Executable
echo && echo
./Code
