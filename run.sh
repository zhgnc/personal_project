#!/bin/bash
set -euo pipefail

info()    { echo -e "\e[1;4;32m$*\e[0m"; }   # Green, bold, underlined
error()   { echo -e "\e[1;4;31m$*\e[0m"; }   # Red, bold, underlined

ROOT_DIR=$(pwd)
EXECUTABLE="$ROOT_DIR/build/MyCode.exe"

if [ ! -f "$EXECUTABLE" ]; then
  error "Executable not found. Please build first."
  exit 1
fi

info "Running $EXECUTABLE"
echo
"$EXECUTABLE"