#!/bin/bash

mkdir -p build

clang -g `sdl2-config --cflags --static-libs` source/*.c -o build/ananke
