#!/bin/sh

cmake -B build-arm64 -DCMAKE_BUILD_TYPE=Release
cmake --build build-arm64 -j$(nproc)
