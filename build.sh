#!/bin/bash

pushd source/engine/stygatore > /dev/null
make
popd > /dev/null

cp source/engine/stygatore/build/stygatore build/

./build/stygatore source/engine/generated

echo "build/styx_platform <- source/platform.c"
clang -Isource/engine -Isource/engine/ext -D_THREAD_SAFE -Wno-incompatible-pointer-types-discards-qualifiers ./source/platform.c -L/opt/homebrew/lib -lsdl2 -o build/styx_platform
