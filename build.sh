#!/bin/bash

compile() {
	odin build source -collection:ext=./engine/ext -collection:engine=./engine -debug -out:build/ananke
}

run() {
	pushd build > /dev/null
	./ananke
	popd > /dev/null
}

mkdir -p build

if [[ "$1" == "run" ]]; then
	compile
	run
else
	echo "Compiling Ananke"
	compile
fi