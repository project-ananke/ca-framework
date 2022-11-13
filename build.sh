#!/bin/bash

mkdir -p build

# files=$(find . -name "*.c")
# echo clang -g `sdl2-config --cflags --static-libs` -Iext/lua544  -o build/ananke

odin build engine -collection:ext=./ext -out:build/ananke