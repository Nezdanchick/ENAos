#!/usr/bin/env bash
set -e

cd "$(dirname "${BASH_SOURCE[0]}")"

if [ ! -d "build" ] || [ ! -f "build/Makefile" ]; then
    cmake -B build -S .
fi

TARGET="${1:-run}"
cmake --build build --target "$TARGET"
