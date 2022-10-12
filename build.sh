#!/bin/bash

pushd source/engine/stygatore > /dev/null
go build
popd > /dev/null

mkdir -p build
mv source/engine/stygatore/stygatore build/

./build/stygatore source/engine/generated

echo "build/styx_platform <- source/platform.c"
clang -Isource/engine -Isource/engine/ext `sdl2-config --cflags --static-libs` -Wno-incompatible-pointer-types-discards-qualifiers ./source/platform.c -o build/styx_platform
