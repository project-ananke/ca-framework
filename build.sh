#!/bin/bash

clang -Isource/engine -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE -Wno-incompatible-pointer-types-discards-qualifiers ./source/platform.c -L/opt/homebrew/lib -lsdl2 -o build/styx_platform
