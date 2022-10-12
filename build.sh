#!/bin/bash

pushd source/engine/stygatore > /dev/null
go build
popd > /dev/null

mv source/engine/stygatore/stygatore build/

./build/stygatore source/engine/generated

echo "build/styx_platform <- source/platform.c"
clang -Isource/engine -Isource/engine/ext `sdl2-config --cflags --libs` -Wno-incompatible-pointer-types-discards-qualifiers ./source/platform.c -o build/styx_platform
